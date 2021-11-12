#include <aarch64/intrinsic.h>
#include <common/spinlock.h>
#include <core/cpu.h>

void init_spinlock(SpinLock *lock, const char *name) {
    lock->locked = 0;
    lock->cpu = NULL;
    lock->name = name;
}

bool try_acquire_spinlock(SpinLock *lock) {
    if (!lock->locked && !__atomic_test_and_set(&lock->locked, __ATOMIC_ACQUIRE)) {
        lock->cpu = thiscpu();
        return true;
    } else {
        return false;
    }
}

/*
 * Acquire a spinlock.
 * Caller should not hold this lock before calling this function.
 * If the lock is held by another cpu, it spins.
 */

void acquire_spinlock(SpinLock *lock) {
    if (holding_spinlock(lock)) {
        PANIC("acquire: lock %s already held\n", lock->name);
    }
    while (!try_acquire_spinlock(lock)) {}
}

/*
 * Release a spinlock.
 * Caller should hold this lock before calling this function.
 */

void release_spinlock(SpinLock *lock) {
    if (!holding_spinlock(lock)) {
        PANIC("release: lock %s not held\n", lock->name);
    }
    lock->cpu = NULL;
    __atomic_clear(&lock->locked, __ATOMIC_RELEASE);
}

void wait_spinlock(SpinLock *lock) {
    acquire_spinlock(lock);
    release_spinlock(lock);
}

/*
 * For debug: Check whether a cpu holds this lock.
 */

bool holding_spinlock(SpinLock *lock) {
    return lock->locked && lock->cpu == thiscpu();
}
