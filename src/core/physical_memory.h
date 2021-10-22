#pragma once

#ifndef _CORE_MEMORY_MANAGE_
#define _CORE_MEMORY_MANAGE_

#include <common/spinlock.h>

typedef struct {
    void *struct_ptr;
    void (*page_init)(void *datastructure_ptr, void *start, void *end);
    void *(*page_alloc)(void *datastructure_ptr);
    void (*page_free)(void *datastructure_ptr, void *page_address);
    SpinLock lock;
} PMemory;

typedef struct {
    void *next;
} FreeListNode;

void init_memory_manager(void);
void free_range(void *start, void *end);
void *kalloc(void);
void kfree(void *page_address);

#endif