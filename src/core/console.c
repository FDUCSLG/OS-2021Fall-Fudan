#include <aarch64/intrinsic.h>
#include <common/format.h>
#include <core/console.h>
#include <common/spinlock.h>

typedef struct {
    SpinLock lock;
    CharDevice device;
} ConsoleContext;

static ConsoleContext ctx;

void init_console() {
    init_spinlock(&ctx.lock, "console");
    init_uart_char_device(&ctx.device);
}

#define PANIC_BAR_CHAR   '='
#define PANIC_BAR_LENGTH 32

static bool panicked_flag = false;

// check whether other cores have already panicked.
// if true, just terminate itself.
// note: you must hold `ctx.lock` while checking.
static void check_panicked() {
    bool panicked = __atomic_load_n(&panicked_flag, __ATOMIC_ACQUIRE);

    if (panicked) {
        release_spinlock(&ctx.lock);
        no_return();
    }
}

void puts(const char *str) {
    acquire_spinlock(&ctx.lock);
    check_panicked();

    while (*str != '\0') {
        ctx.device.put(*str++);
    }

    // add a trailing newline.
    ctx.device.put(NEWLINE);

    release_spinlock(&ctx.lock);
}

static void _put_char(void *_ctx, char c) {
    (void)_ctx;
    ctx.device.put(c);
}

void vprintf(const char *fmt, va_list arg) {
    acquire_spinlock(&ctx.lock);
    check_panicked();

    vformat(_put_char, NULL, fmt, arg);

    release_spinlock(&ctx.lock);
}

void printf(const char *fmt, ...) {
    va_list arg;
    va_start(arg, fmt);
    vprintf(fmt, arg);
    va_end(arg);
}

NORETURN void _panic(const char *file, usize line, const char *fmt, ...) {
    acquire_spinlock(&ctx.lock);
    check_panicked();

    // mark the whole system as panicked.
    __atomic_store_n(&panicked_flag, true, __ATOMIC_RELEASE);

    // print messages.

    for (usize i = 0; i < PANIC_BAR_LENGTH; i++)
        ctx.device.put(PANIC_BAR_CHAR);
    ctx.device.put(NEWLINE);

    format(_put_char, NULL, "KERNEL PANIC at core %zu:\n", cpuid());
    format(_put_char, NULL, "file: %s\n", file);
    format(_put_char, NULL, "line: %zu\n", line);

    va_list arg;
    va_start(arg, fmt);
    vformat(_put_char, NULL, fmt, arg);
    va_end(arg);

    // add a trailing newline for message.
    ctx.device.put(NEWLINE);

    release_spinlock(&ctx.lock);

    no_return();
}
