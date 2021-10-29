#include "map.hpp"

namespace {
struct Lock {
    Lock() : lock(mutex, std::defer_lock) {}

    std::mutex mutex;
    std::unique_lock<std::mutex> lock;
};

Map<struct SpinLock *, Lock> map;
}  // namespace

extern "C" {
void init_spinlock(struct SpinLock *lock, const char *name [[maybe_unused]]) {
    map.try_add(lock);
}

void acquire_spinlock(struct SpinLock *lock) {
    map[lock].lock.lock();
}

void release_spinlock(struct SpinLock *lock) {
    map[lock].lock.unlock();
}

bool holding_spinlock(struct SpinLock *lock) {
    return map[lock].lock.owns_lock();
}
}
