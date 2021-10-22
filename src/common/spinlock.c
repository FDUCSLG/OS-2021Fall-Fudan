#include <aarch64/intrinsic.h>
#include <common/spinlock.h>

/* 
 * Init a spinlock and assign a name to it.
 */
void init_spinlock(SpinLock *lock, char *name) {
    lock->locked = 0;
    lock->cpu = 0;
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
        PANIC("acquire: lock already held\n");
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
	lock->cpu = 0;
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