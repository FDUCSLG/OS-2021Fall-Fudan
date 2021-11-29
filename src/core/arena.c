#include <core/arena.h>
#include <core/console.h>
#include <core/physical_memory.h>

void init_arena(Arena *arena, usize object_size, ArenaPageAllocator allocator) {
    asserts(object_size >= ARENA_MIN_OBJECT_SIZE, "object_size = %zu is too small", object_size);
    asserts(object_size <= ARENA_PAGE_CAPACITY, "object_size = %zu is too large", object_size);

    init_spinlock(&arena->lock, "arena");
    arena->allocator = allocator;

    arena->object_size = object_size;
    arena->max_count = ARENA_PAGE_CAPACITY / object_size;
    arena->pages = NULL;

    arena->num_objects = 0;
    arena->num_pages = 0;
}

void clear_arena(Arena *arena) {
    acquire_spinlock(&arena->lock);

    if (arena->pages) {
        ListNode *head = &arena->pages->list;
        ListNode *current = head;

        do {
            ListNode *next = current->next;
            arena->allocator.free(container_of(current, ArenaPage, list));
            current = next;
        } while (current != head);

        arena->pages = NULL;
        arena->num_objects = 0;
        arena->num_pages = 0;
    }

    release_spinlock(&arena->lock);
}

static void init_arena_page(Arena *arena, ArenaPage *page) {
    page->arena = arena;
    init_list_node(&page->list);
    init_bitmap(page->used, ARENA_BITMAP_SIZE);
    page->count = 0;
}

static void insert_page(Arena *arena, ArenaPage *page) {
    if (arena->pages) {
        ListNode *head = merge_list(&page->list, &arena->pages->list);
        arena->pages = container_of(head, ArenaPage, list);
    } else
        arena->pages = page;
}

static ArenaPage *add_page(Arena *arena) {
    ArenaPage *page = arena->allocator.allocate();
    asserts((u64)page % ARENA_PAGE_SIZE == 0, "page address must be aligned on ARENA_PAGE_SIZE");

    if (page != NULL) {
        init_arena_page(arena, page);
        insert_page(arena, page);
        arena->num_pages++;
    }

    return page;
}

void *alloc_object(Arena *arena) {
    acquire_spinlock(&arena->lock);

    // if there's no available slot, add a new page.
    if (!arena->pages || arena->pages->count >= arena->max_count)
        add_page(arena);

    // find an empty slot in page.
    ArenaPage *page = arena->pages;
    void *object = NULL;
    for (usize index = 0; index < arena->max_count; index++) {
        if (bitmap_get(page->used, index) == false) {
            object = page->data + index * arena->object_size;
            bitmap_set(page->used, index);
            page->count++;
            arena->num_objects++;
            break;
        }
    }

    asserts(object != NULL, "arena failed to allocate object");

    // if current page is full, move list head to the next page.
    if (page->count >= arena->max_count)
        arena->pages = container_of(page->list.next, ArenaPage, list);

    release_spinlock(&arena->lock);
    return object;
}

static INLINE ArenaPage *get_container_page(void *object) {
    return (ArenaPage *)round_down((u64)object, ARENA_PAGE_SIZE);
}

static void move_page_to_head(Arena *arena, ArenaPage *page) {
    if (arena->pages != page) {
        detach_from_list(&page->list);
        insert_page(arena, page);
    }
}

void free_object(void *object) {
    ArenaPage *page = get_container_page(object);
    Arena *arena = page->arena;

    acquire_spinlock(&arena->lock);

    usize offset = (usize)object - (usize)page->data;
    asserts(offset % arena->object_size == 0, "unexpected object offset = %zu", offset);

    usize index = offset / arena->object_size;

    asserts(bitmap_get(page->used, index) == true, "double free object at index = %zu", index);
    bitmap_clear(page->used, index);
    page->count--;
    arena->num_objects--;

    // now page must have empty slots. Move it to the list head to make sure
    // these slots can be used in following allocations.
    move_page_to_head(arena, page);

    release_spinlock(&arena->lock);
}

void arena_test() {
    puts("arena_test begin.");

    typedef struct {
        u64 cookie;
        u64 data[128];
    } Payload;

    Arena arena;
    ArenaPageAllocator allocator = {.allocate = kalloc, .free = kfree};
    init_arena(&arena, sizeof(Payload), allocator);
    puts("init_arena okay.");

    Payload *payloads[128];
    for (usize i = 0; i < 128; i++) {
        payloads[i] = alloc_object(&arena);
        assert(payloads[i] != NULL);

        payloads[i]->cookie = i * 0x19260817;
        for (usize j = 0; j < 128; j++) {
            payloads[i]->data[j] = i * j;
        }

        assert(arena.num_objects == i + 1);
    }
    puts("allocate_object okay.");

    for (usize i = 0; i < 128; i += 2) {
        assert(payloads[i]->cookie == i * 0x19260817);
        free_object(payloads[i]);
    }
    assert(arena.num_objects == 64);
    puts("free_object okay.");

    Payload *ptr = alloc_object(&arena);
    assert(ptr != NULL);
    assert(arena.num_objects == 65);
    puts("re-allocate_object okay.");

    clear_arena(&arena);
    puts("clear_arena okay.");
}
