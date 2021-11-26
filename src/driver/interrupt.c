#include <aarch64/arm.h>
#include <aarch64/intrinsic.h>
#include <core/console.h>
#include <driver/base.h>
#include <driver/clock.h>
#include <driver/interrupt.h>
#include <driver/irq.h>

#define IRQ_BASIC_PENDING  (MMIO_BASE + 0xB200)
#define IRQ_PENDING_1      (MMIO_BASE + 0xB204)
#define IRQ_PENDING_2      (MMIO_BASE + 0xB208)
#define FIQ_CONTROL        (MMIO_BASE + 0xB20C)
#define ENABLE_IRQS_1      (MMIO_BASE + 0xB210)
#define ENABLE_IRQS_2      (MMIO_BASE + 0xB214)
#define ENABLE_BASIC_IRQS  (MMIO_BASE + 0xB218)
#define DISABLE_IRQS_1     (MMIO_BASE + 0xB21C)
#define DISABLE_IRQS_2     (MMIO_BASE + 0xB220)
#define DISABLE_BASIC_IRQS (MMIO_BASE + 0xB224)

#define GPU_INT_ROUTE   (LOCAL_BASE + 0xC)
#define GPU_IRQ2CORE(i) (i)

#define IRQ_SRC_CORE(i)   (LOCAL_BASE + 0x60 + 4 * (i))
#define IRQ_SRC_TIMER     (1 << 11)  // global timer
#define IRQ_SRC_GPU       (1 << 8)
#define IRQ_SRC_CNTPNSIRQ (1 << 1)  // CPU clock
#define FIQ_SRC_CORE(i)   (LOCAL_BASE + 0x70 + 4 * (i))

typedef struct {
    InterruptHandler handler[NUM_IRQ_TYPES];
} InterruptContext;

static InterruptContext ctx;

void init_interrupt() {
    for (usize i = 0; i < NUM_IRQ_TYPES; i++) {
        ctx.handler[i] = 0;
    }
    put32(ENABLE_IRQS_1, AUX_INT);
    put32(ENABLE_IRQS_2, VC_ARASANSDIO_INT);
    device_put_u32(GPU_INT_ROUTE, GPU_IRQ2CORE(0));
}

void set_interrupt_handler(InterruptType type, InterruptHandler handler) {
    device_put_u32(ENABLE_IRQS_1 + 4 * (type / 32), 1 << (type % 32));
    ctx.handler[type] = handler;
}

void interrupt_global_handler() {
    u32 source = device_get_u32(IRQ_SRC_CORE(cpuid()));

    if (source & IRQ_SRC_CNTPNSIRQ) {
        source ^= IRQ_SRC_CNTPNSIRQ;

        invoke_clock_handler();
    }

    if (source & IRQ_SRC_GPU) {
        source ^= IRQ_SRC_GPU;

        u64 map = device_get_u32(IRQ_PENDING_1) | (((u64)device_get_u32(IRQ_PENDING_2)) << 32);
        for (usize i = 0; i < NUM_IRQ_TYPES; i++) {
            if ((map >> i) & 1) {
                if (ctx.handler[i])
                    ctx.handler[i]();
                else
                    PANIC("unknown interrupt type: %d", i);
            }
        }
    }

    if (source != 0)
        PANIC("unknown interrupt sources: %x", source);
}
