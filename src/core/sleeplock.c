#include <core/proc.h>
#include <core/sleeplock.h>

void init_sleeplock(SleepLock *lock, const char *name) {
    init_spinlock(&lock->lock, name);
    lock->locked = false;
}

void acquire_sleeplock(SleepLock *lock) {
    acquire_spinlock(&lock->lock);
    while (lock->locked) {
        sleep(lock, &lock->lock);
    }
    lock->locked = true;
    release_spinlock(&lock->lock);
}

void release_sleeplock(SleepLock *lock) {
    acquire_spinlock(&lock->lock);
    lock->locked = false;
    release_spinlock(&lock->lock);
    wakeup(lock);
}
