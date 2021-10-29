#pragma once

#include <aarch64/mmu.h>
#include <common/bitmap.h>
#include <common/list.h>
#include <common/spinlock.h>

#define ARENA_PAGE_SIZE       PAGE_SIZE
#define ARENA_MIN_OBJECT_SIZE 16
#define ARENA_BITMAP_SIZE     (ARENA_PAGE_SIZE / ARENA_MIN_OBJECT_SIZE)

typedef struct {
    void *(*allocate)();
    void (*free)(void *page);
} ArenaPageAllocator;

struct Arena;

typedef struct {
    struct Arena *arena;
    ListNode list;
    Bitmap(used, ARENA_BITMAP_SIZE);
    usize count;

    // flexible array member: <https://en.wikipedia.org/wiki/Flexible_array_member>
    u8 data[];
} ArenaPage;

#define ARENA_PAGE_CAPACITY (ARENA_PAGE_SIZE - sizeof(ArenaPage))

typedef struct Arena {
    SpinLock lock;
    ArenaPageAllocator allocator;

    usize object_size, max_count;
    ArenaPage *pages;

    usize num_objects;  // the number of allocated objects.
    usize num_pages;    // the number of allocated pages.
} Arena;

void init_arena(Arena *arena, usize object_size, ArenaPageAllocator allocator);

// free all objects allocated by `arena`. All memory pages associated with
// it will be freed as well.
void clear_arena(Arena *arena);

// NOTE: allocated object memory is uninitialized.
void *alloc_object(Arena *arena);

// find which arena `object` belongs to and mark the memory used by `object` as
// free.
void free_object(void *object);

void arena_test();
