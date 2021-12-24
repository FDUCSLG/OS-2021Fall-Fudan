#include <aarch64/intrinsic.h>
#include <common/format.h>
#include <common/spinlock.h>
#include <core/console.h>
#include <core/proc.h>
#include <core/sched.h>
#include <driver/uart.h>
#include <fs/inode.h>

typedef struct {
    SpinLock lock;
    CharDevice device;
} ConsoleContext;
static SpinLock conslock;

static ConsoleContext ctx;

void init_console() {
    init_spinlock(&ctx.lock, "console");
    init_uart_char_device(&ctx.device);
    init_spinlock(&conslock, "conslock");
}

#define PANIC_BAR_CHAR   '='
#define PANIC_BAR_LENGTH 32

static bool panicked_flag = false;

// check whether other CPUs have already panicked.
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

    format(_put_char, NULL, "KERNEL PANIC at CPU %zu:\n", cpuid());
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

#define CONSOLE 1

static int panicked = -1;

#define INPUT_BUF 128
struct {
    char buf[INPUT_BUF];
    usize r;  // Read index
    usize w;  // Write index
    usize e;  // Edit index
} input;
#define C(x)      ((x) - '@')  // Control-x
#define BACKSPACE 0x100

static void consputc(int c) {
    if (c == BACKSPACE) {
        uart_put_char('\b');
        uart_put_char(' ');
        uart_put_char('\b');
    } else
        uart_put_char(c);
}

isize console_write(Inode *ip, char *buf, isize n) {
    inodes.unlock(ip);
    acquire_spinlock(&conslock);
    for (int i = 0; i < n; i++)
        consputc(buf[i] & 0xff);
    release_spinlock(&conslock);
    inodes.lock(ip);
    return n;
}

isize console_read(Inode *ip, char *dst, isize n) {
    inodes.unlock(ip);
    usize target = n;
    acquire_spinlock(&conslock);
    while (n > 0) {
        while (input.r == input.w) {
            if (thiscpu()->proc->killed) {
                release_spinlock(&conslock);
                inodes.lock(ip);
                return -1;
            }
            sleep(&input.r, &conslock);
        }
        int c = input.buf[input.r++ % INPUT_BUF];
        if (c == C('D')) {  // EOF
            if (n < target) {
                // Save ^D for next time, to make sure
                // caller gets a 0-byte result.
                input.r--;
            }
            break;
        }
        *dst++ = c;
        --n;
        if (c == '\n')
            break;
    }
    release_spinlock(&conslock);
    inodes.lock(ip);

    return target - n;
}

void console_intr(char (*getc)()) {
    int c, doprocdump = 0;

    acquire_spinlock(&conslock);
    if (panicked >= 0) {
        release_spinlock(&conslock);
        while (1)
            ;
    }

    while ((c = getc()) != 0xff) {
        switch (c) {
            case C('P'):  // Process listing.
                // procdump() locks cons.lock indirectly; invoke later
                doprocdump = 1;
                break;
            case C('U'):  // Kill line.
                while (input.e != input.w && input.buf[(input.e - 1) % INPUT_BUF] != '\n') {
                    input.e--;
                    consputc(BACKSPACE);
                }
                break;
            case C('H'):
            case '\x7f':  // Backspace
                if (input.e != input.w) {
                    input.e--;
                    consputc(BACKSPACE);
                }
                break;
            default:
                if (c != 0 && input.e - input.r < INPUT_BUF) {
                    c = (c == '\r') ? '\n' : c;
                    input.buf[input.e++ % INPUT_BUF] = c;
                    consputc(c);
                    if (c == '\n' || c == C('D') || input.e == input.r + INPUT_BUF) {
                        input.w = input.e;
                        wakeup(&input.r);
                    }
                }
                break;
        }
    }
    release_spinlock(&conslock);
}
