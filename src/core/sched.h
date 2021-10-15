#pragma once

#include <common/defines.h>
#include <core/console.h>
#include <aarch64/intrinsic.h>

struct scheduler;

// enum sched_obj_type {
//     PROCESS,
//     SCHEDULER
// };

// struct sched_obj {
//     enum sched_obj_type type;
// };

struct sched_op {
    void (*scheduler)();
    struct proc *(*alloc_pcb)();
    void (*sched)();
};

struct scheduler {
    // struct sched_obj sched;
    struct sched_op *op;
    struct context *context;
};

extern struct scheduler simple_scheduler;

struct cpu {
    struct scheduler *scheduler;
    struct proc *proc;
};
#define NCPU       4    /* maximum number of CPUs */
extern struct cpu cpus[NCPU];

static inline struct cpu *thiscpu()
{
    return &cpus[cpuid()];
}

static inline void init_cpu(struct scheduler *scheduler)
{
    thiscpu()->scheduler = scheduler;
}

void init_sched();


static inline void enter_scheduler() {
	assert(thiscpu()->scheduler != NULL);
    thiscpu()->scheduler->op->scheduler();
}

static inline void sched() {
    thiscpu()->scheduler->op->sched();
}

static inline struct proc *alloc_pcb() {
    assert(thiscpu()->scheduler != NULL);
	assert(thiscpu()->scheduler->op != NULL);
	assert(thiscpu()->scheduler->op->alloc_pcb != NULL);
    return thiscpu()->scheduler->op->alloc_pcb();
}