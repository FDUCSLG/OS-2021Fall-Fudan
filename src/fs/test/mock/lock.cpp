#include "lock_config.hpp"
#include "map.hpp"

#include <condition_variable>

namespace {

struct Mutex {
    bool locked;
    std::mutex mutex;

    void lock() {
        mutex.lock();
        locked = true;
    }

    void unlock() {
        locked = false;
        mutex.unlock();
    }
};

struct Signal {
    // use a pointer to avoid `pthread_cond_destroy` blocking process exit.
    std::condition_variable_any *cv;

    Signal() {
        cv = new std::condition_variable_any;
    }
};

Map<void *, Mutex> mtx_map;
Map<void *, Signal> sig_map;

}  // namespace

extern "C" {
void init_spinlock(struct SpinLock *lock, const char *name [[maybe_unused]]) {
    mtx_map.try_add(lock);
}

void acquire_spinlock(struct SpinLock *lock) {
    mtx_map[lock].lock();
}

void release_spinlock(struct SpinLock *lock) {
    mtx_map[lock].unlock();
}

bool holding_spinlock(struct SpinLock *lock) {
    return mtx_map[lock].locked;
}

void init_sleeplock(struct SleepLock *lock, const char *name [[maybe_unused]]) {
    mtx_map.try_add(lock);
}

void acquire_sleeplock(struct SleepLock *lock) {
    mtx_map[lock].lock();
}

void release_sleeplock(struct SleepLock *lock) {
    mtx_map[lock].unlock();
}

void _fs_test_sleep(void *chan, struct SpinLock *lock) {
    sig_map.safe_get(chan).cv->wait(mtx_map[lock].mutex);
}

void _fs_test_wakeup(void *chan) {
    sig_map.safe_get(chan).cv->notify_all();
}
}
