#pragma once

#include <aarch64/intrinsic.h>
#include <common/defines.h>
#include <common/spinlock.h>
#include <core/console.h>
#include <core/proc.h>

#define MULTI_SCHEDULER
#ifndef MULTI_SCHEDULER
struct scheduler;

// enum sched_obj_type {
//     PROCESS,
//     SCHEDULER
// };

// struct sched_obj {
//     enum sched_obj_type type;
// };

struct sched_op {
    void (*init)();
    void (*scheduler)();
    struct proc *(*alloc_pcb)();
    void (*sched)();
    void (*acquire_lock)();
    void (*release_lock)();
};

struct scheduler {
    // struct sched_obj sched;
    struct sched_op *op;
    struct context *context;
    struct proc ptable[NPROC];
    SpinLock lock;
};

extern struct scheduler simple_scheduler;

struct cpu {
    struct scheduler *scheduler;
    struct proc *proc;
};
#define NCPU 4 /* maximum number of CPUs */
extern struct cpu cpus[NCPU];

static INLINE struct cpu *thiscpu() {
    return &cpus[cpuid()];
}

static INLINE void init_sched() {
    // thiscpu()->scheduler->op->init();
    simple_scheduler.op->init();
}

static INLINE void init_cpu(struct scheduler *scheduler) {
    thiscpu()->scheduler = scheduler;
    // init_sched();
}

static INLINE void enter_scheduler() {
    assert(thiscpu()->scheduler != NULL);
    thiscpu()->scheduler->op->scheduler();
}

static INLINE void sched() {
    thiscpu()->scheduler->op->sched();
}

static INLINE struct proc *alloc_pcb() {
    assert(thiscpu()->scheduler != NULL);
    assert(thiscpu()->scheduler->op != NULL);
    assert(thiscpu()->scheduler->op->alloc_pcb != NULL);
    return thiscpu()->scheduler->op->alloc_pcb();
}

static INLINE void acquire_sched_lock() {
    thiscpu()->scheduler->op->acquire_lock();
}

static INLINE void release_sched_lock() {
    thiscpu()->scheduler->op->release_lock();
}
#else

struct scheduler;
struct sched_op {
    void (*init)();
    void (*scheduler)(struct scheduler *this);
    struct proc *(*alloc_pcb)(struct scheduler *this);
    void (*sched)(struct scheduler *this);
    void (*acquire_lock)(struct scheduler *this);
    void (*release_lock)(struct scheduler *this);
    struct context *(*get_context)(struct scheduler *this);
};
extern struct sched_op simple_op;

#define NCPU 4 /* maximum number of CPUs */

struct scheduler {
    // struct sched_obj sched;
    struct sched_op *op;
    struct context *context[NCPU];
    struct {
        struct proc proc[NPROC];
        SpinLock lock;
    } ptable;
    int pid;
    struct scheduler *parent;
    struct container *cont;
};

struct cpu {
    struct scheduler *scheduler;
    struct proc *proc;
};
extern struct cpu cpus[NCPU];

static INLINE struct cpu *thiscpu() {
    return &cpus[cpuid()];
}

static INLINE void init_sched() {
    // thiscpu()->scheduler->op->init();
    // simple_scheduler.op->init();
}

static INLINE void init_cpu(struct scheduler *scheduler) {
    thiscpu()->scheduler = scheduler;
    //     init_sched();
}

static INLINE void enter_scheduler() {
    assert(thiscpu()->scheduler != NULL);
    assert(thiscpu()->scheduler->op != NULL);
    assert(thiscpu()->scheduler->op->scheduler != NULL);
    thiscpu()->scheduler->op->scheduler(thiscpu()->scheduler);
}

static INLINE void sched() {
    thiscpu()->scheduler->op->sched(thiscpu()->scheduler);
}

static INLINE struct proc *alloc_pcb() {
    assert(thiscpu()->scheduler != NULL);
    assert(thiscpu()->scheduler->op != NULL);
    assert(thiscpu()->scheduler->op->alloc_pcb != NULL);
    return thiscpu()->scheduler->op->alloc_pcb(thiscpu()->scheduler);
}

static INLINE void acquire_sched_lock() {
    thiscpu()->scheduler->op->acquire_lock(thiscpu()->scheduler);
}

static INLINE void release_sched_lock() {
    thiscpu()->scheduler->op->release_lock(thiscpu()->scheduler);
}
#endif
