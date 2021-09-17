#include <aarch64/intrinsic.h>

void delay_us(uint64_t n) {
    uint64_t freq = get_timer_freq();
    uint64_t end = get_timestamp(), now;
    end += freq / 1000000 * n;

    do {
        now = get_timestamp();
    } while (now <= end);
}
