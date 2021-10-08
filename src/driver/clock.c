#include <aarch64/intrinsic.h>
#include <core/console.h>
#include <driver/base.h>
#include <driver/clock.h>

#define CORE_CLOCK_CTRL(id) (LOCAL_BASE + 0x40 + 4 * (id))
#define CORE_CLOCK_ENABLE   (1 << 1)

typedef struct {
    u64 one_ms;
    ClockHandler handler;
} ClockContext;

static ClockContext ctx;

void init_clock() {
    ctx.one_ms = get_clock_frequency() / 1000;

    // reserve one second for the first time.
    asm volatile("msr cntp_ctl_el0, %[x]" ::[x] "r"(1ll));
    reset_clock(1000);

    device_put_u32(CORE_CLOCK_CTRL(cpuid()), CORE_CLOCK_ENABLE);
}

void reset_clock(u64 countdown_ms) {
    asm volatile("msr cntp_tval_el0, %[x]" ::[x] "r"(countdown_ms * ctx.one_ms));
}

void set_clock_handler(ClockHandler handler) {
    ctx.handler = handler;
}

void invoke_clock_handler() {
    if (!ctx.handler)
        PANIC("no clock handler");

    ctx.handler();
}
