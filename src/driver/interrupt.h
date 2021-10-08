#pragma once

#ifndef _DRIVER_INTERRUPT_H_
#define _DRIVER_INTERRUPT_H_

#include <common/defines.h>

// "IRQ" is the shorthand for "interrupt".
#define NUM_IRQ_TYPES 64

typedef enum {
    IRQ_AUX = 29,
    IRQ_SDIO = 56,
    IRQ_ARASANSDIO = 62,
} InterruptType;

typedef void (*InterruptHandler)(void);

void init_interrupt();
void set_interrupt_handler(InterruptType type, InterruptHandler handler);
void interrupt_global_handler();
static inline void test_kernel_interrupt() {
    arch_enable_trap();
    while (1) {
        arch_wfi();
    }
}

#endif
