#include <aarch64/intrinsic.h>
#include <common/spinlock.h>

void init_spinlock(SpinLock *lock) {
    lock->locked = 0;
}

bool try_acquire_spinlock(SpinLock *lock) {
    return !lock->locked && !__atomic_test_and_set(&lock->locked, __ATOMIC_ACQUIRE);
}

void acquire_spinlock(SpinLock *lock) {
    while (!try_acquire_spinlock(lock)) {}
}

void release_spinlock(SpinLock *lock) {
    __atomic_clear(&lock->locked, __ATOMIC_RELEASE);
}

void wait_spinlock(SpinLock *lock) {
    acquire_spinlock(lock);
    release_spinlock(lock);
}
