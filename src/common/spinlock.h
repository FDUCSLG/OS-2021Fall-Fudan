#pragma once

#include <common/defines.h>

typedef struct SpinLock {
    volatile bool locked;
    struct cpu *cpu;
    const char *name;
} SpinLock;

void init_spinlock(SpinLock *lock, const char *);

bool try_acquire_spinlock(SpinLock *lock);
void acquire_spinlock(SpinLock *lock);
void release_spinlock(SpinLock *lock);
void wait_spinlock(SpinLock *lock);
bool holding_spinlock(SpinLock *lock);
