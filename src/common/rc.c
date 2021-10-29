#include <common/rc.h>

void init_rc(RefCount *rc) {
    rc->count = 0;
}

void increment_rc(RefCount *rc) {
    __atomic_fetch_add(&rc->count, 1, __ATOMIC_ACQ_REL);
}

bool decrement_rc(RefCount *rc) {
    return __atomic_sub_fetch(&rc->count, 1, __ATOMIC_ACQ_REL) <= 0;
}
