extern "C" {
#include <common/defines.h>
}

#include "map.hpp"

namespace {
Map<struct Arena *, usize> map;
Map<u8 *, u8 *> ref;
}  // namespace

extern "C" {
typedef struct {
    void *(*allocate)();
    void (*free)(void *page);
} ArenaPageAllocator;

void *kalloc() {
    // we need to ensure the return pointer address is a multiple of 4096.
    u8 *p = reinterpret_cast<u8 *>(malloc(10000));
    usize i = reinterpret_cast<usize>(p);
    usize j = (i + 4095) / 4096 * 4096;
    u8 *q = reinterpret_cast<u8 *>(j);
    ref.add(q, p);
    return q;
}

void kfree(void *ptr) {
    free(ref[reinterpret_cast<u8 *>(ptr)]);
}

void init_arena(Arena *arena, usize object_size, ArenaPageAllocator allocator [[maybe_unused]]) {
    map.add(arena, object_size);
}

void *alloc_object(Arena *arena) {
    return malloc(map[arena]);
}

void free_object(void *object) {
    free(object);
}
}
