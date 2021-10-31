#include "map.hpp"

namespace {
struct Lock {
    bool locked;
    std::mutex mutex;
};

Map<struct SpinLock *, Lock> map;
}  // namespace

extern "C" {
void init_spinlock(struct SpinLock *lock, const char *name [[maybe_unused]]) {
    map.try_add(lock);
}

void acquire_spinlock(struct SpinLock *lock) {
    auto &m = map[lock];
    m.mutex.lock();
    m.locked = true;
}

void release_spinlock(struct SpinLock *lock) {
    auto &m = map[lock];
    m.locked = false;
    m.mutex.unlock();
}

bool holding_spinlock(struct SpinLock *lock) {
    return map[lock].locked;
}
}
