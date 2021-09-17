#pragma once

#ifndef _AARCH64_ARCH_H_
#define _AARCH64_ARCH_H_

#include <common/types.h>

static inline size_t cpuid() {
    uint64_t id;
    asm volatile("mrs %[x], mpidr_el1" : [x] "=r"(id));
    return id & 0xff;
}

// instruct compiler not to reorder instructions around the fence.
static inline void compiler_fence() {
    asm volatile("" ::: "memory");
}

static inline uint64_t get_timer_freq() {
    uint64_t result;
    asm volatile("mrs %[freq], cntfrq_el0" : [freq] "=r"(result));
    return result;
}

static inline uint64_t get_timestamp() {
    uint64_t result;
    compiler_fence();
    asm volatile("mrs %[cnt], cntpct_el0" : [cnt] "=r"(result));
    compiler_fence();
    return result;
}

// instruction synchronization barrier.
static inline void arch_isb() {
    asm volatile("isb" ::: "memory");
}

// data synchronization barrier.
static inline void arch_dsb_sy() {
    asm volatile("dsb sy" ::: "memory");
}

static inline void arch_fence() {
    arch_dsb_sy();
    arch_isb();
}

// for `device_get/put_*`, there's no need to protect them with architectual
// barriers, since they are intended to access device memory regions. These
// regions are already marked as nGnRnE in `kernel_pt`.

static inline void device_put_uint32(uint64_t addr, uint32_t value) {
    compiler_fence();
    *(volatile uint32_t *)addr = value;
    compiler_fence();
}

static inline uint32_t device_get_uint32(uint64_t addr) {
    compiler_fence();
    uint32_t value = *(volatile uint32_t *)addr;
    compiler_fence();
    return value;
}

// read Exception Syndrome Register (EL1).
static inline uint64_t arch_get_esr() {
    uint64_t result;
    arch_fence();
    asm volatile("mrs %[x], esr_el1" : [x] "=r"(result));
    arch_fence();
    return result;
}

// reset Exception Syndrome Register (EL1) to zero.
static inline void arch_reset_esr() {
    arch_fence();
    asm volatile("msr esr_el1, %[x]" : : [x] "r"(0ll));
    arch_fence();
}

// read Exception Link Register (EL1).
static inline uint64_t arch_get_elr() {
    uint64_t result;
    arch_fence();
    asm volatile("mrs %[x], elr_el1" : [x] "=r"(result));
    arch_fence();
    return result;
}

// set vector base (virtual) address register (EL1).
static inline void arch_set_vbar(void *ptr) {
    arch_fence();
    asm volatile("msr vbar_el1, %[x]" : : [x] "r"(ptr));
    arch_fence();
}

// flush TLB entries.
static inline void arch_tlbi_vmalle1is() {
    arch_fence();
    asm volatile("tlbi vmalle1is");
    arch_fence();
}

// set Translation Table Base Register 0 (EL1).
static inline void arch_set_ttbr0(uint64_t addr) {
    arch_fence();
    asm volatile("msr ttbr0_el1, %[x]" : : [x] "r"(addr));
    arch_tlbi_vmalle1is();
}

// set Translation Table Base Register 1 (EL1).
static inline void arch_set_ttbr1(uint64_t addr) {
    arch_fence();
    asm volatile("msr ttbr1_el1, %[x]" : : [x] "r"(addr));
    arch_tlbi_vmalle1is();
}

// set-event instruction.
static inline void arch_sev() {
    asm volatile("sev" ::: "memory");
}

// wait-for-event instruction.
static inline void arch_wfe() {
    asm volatile("wfe" ::: "memory");
}

// wait-for-interrupt instruction.
static inline void arch_wfi() {
    asm volatile("wfi" ::: "memory");
}

// yield instruction.
static inline void arch_yield() {
    asm volatile("yield" ::: "memory");
}

void delay_us(uint64_t n);

#endif
