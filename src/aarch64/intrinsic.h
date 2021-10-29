#pragma once

#include <common/defines.h>

static ALWAYS_INLINE usize cpuid() {
    u64 id;
    asm volatile("mrs %[x], mpidr_el1" : [x] "=r"(id));
    return id & 0xff;
}

// instruct compiler not to reorder instructions around the fence.
static ALWAYS_INLINE void compiler_fence() {
    asm volatile("" ::: "memory");
}

static ALWAYS_INLINE u64 get_clock_frequency() {
    u64 result;
    asm volatile("mrs %[freq], cntfrq_el0" : [freq] "=r"(result));
    return result;
}

static ALWAYS_INLINE u64 get_timestamp() {
    u64 result;
    compiler_fence();
    asm volatile("mrs %[cnt], cntpct_el0" : [cnt] "=r"(result));
    compiler_fence();
    return result;
}

// instruction synchronization barrier.
static ALWAYS_INLINE void arch_isb() {
    asm volatile("isb" ::: "memory");
}

// data synchronization barrier.
static ALWAYS_INLINE void arch_dsb_sy() {
    asm volatile("dsb sy" ::: "memory");
}

static ALWAYS_INLINE void arch_fence() {
    arch_dsb_sy();
    arch_isb();
}

// for `device_get/put_*`, there's no need to protect them with architectual
// barriers, since they are intended to access device memory regions. These
// regions are already marked as nGnRnE in `kernel_pt`.

static ALWAYS_INLINE void device_put_u32(u64 addr, u32 value) {
    compiler_fence();
    *(volatile u32 *)addr = value;
    compiler_fence();
}

static ALWAYS_INLINE u32 device_get_u32(u64 addr) {
    compiler_fence();
    u32 value = *(volatile u32 *)addr;
    compiler_fence();
    return value;
}

// read Exception Syndrome Register (EL1).
static ALWAYS_INLINE u64 arch_get_esr() {
    u64 result;
    arch_fence();
    asm volatile("mrs %[x], esr_el1" : [x] "=r"(result));
    arch_fence();
    return result;
}

// reset Exception Syndrome Register (EL1) to zero.
static ALWAYS_INLINE void arch_reset_esr() {
    arch_fence();
    asm volatile("msr esr_el1, %[x]" : : [x] "r"(0ll));
    arch_fence();
}

// read Exception Link Register (EL1).
static ALWAYS_INLINE u64 arch_get_elr() {
    u64 result;
    arch_fence();
    asm volatile("mrs %[x], elr_el1" : [x] "=r"(result));
    arch_fence();
    return result;
}

// set vector base (virtual) address register (EL1).
static ALWAYS_INLINE void arch_set_vbar(void *ptr) {
    arch_fence();
    asm volatile("msr vbar_el1, %[x]" : : [x] "r"(ptr));
    arch_fence();
}

// flush TLB entries.
static ALWAYS_INLINE void arch_tlbi_vmalle1is() {
    arch_fence();
    asm volatile("tlbi vmalle1is");
    arch_fence();
}

// set Translation Table Base Register 0 (EL1).
static ALWAYS_INLINE void arch_set_ttbr0(u64 addr) {
    arch_fence();
    asm volatile("msr ttbr0_el1, %[x]" : : [x] "r"(addr));
    arch_tlbi_vmalle1is();
}

// set Translation Table Base Register 1 (EL1).
static ALWAYS_INLINE void arch_set_ttbr1(u64 addr) {
    arch_fence();
    asm volatile("msr ttbr1_el1, %[x]" : : [x] "r"(addr));
    arch_tlbi_vmalle1is();
}

// set-event instruction.
static ALWAYS_INLINE void arch_sev() {
    asm volatile("sev" ::: "memory");
}

// wait-for-event instruction.
static ALWAYS_INLINE void arch_wfe() {
    asm volatile("wfe" ::: "memory");
}

// wait-for-interrupt instruction.
static ALWAYS_INLINE void arch_wfi() {
    asm volatile("wfi" ::: "memory");
}

// yield instruction.
static ALWAYS_INLINE void arch_yield() {
    asm volatile("yield" ::: "memory");
}

static ALWAYS_INLINE void arch_enable_trap() {
    asm volatile("msr daif, %[x]" ::[x] "r"(0ll));
}

static ALWAYS_INLINE void arch_disable_trap() {
    asm volatile("msr daif, %[x]" ::[x] "r"(0xfll << 6));
}

void delay_us(u64 n);
