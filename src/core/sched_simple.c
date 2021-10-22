#include <core/sched.h>
#include <core/console.h>
#include <core/proc.h>
#include <core/virtual_memory.h>
#include <common/spinlock.h>

struct {
    struct proc proc[NPROC];
    SpinLock lock;
} ptable /* TODO: Lab5 multicore: Add locks where needed in this file or others */;

static void scheduler_simple();
static struct proc *alloc_pcb_simple();
static void sched_simple();
static void init_sched_simple();
static void acquire_ptable_lock();
static void release_ptable_lock();
struct sched_op simple_op = {.scheduler = scheduler_simple,
                             .alloc_pcb = alloc_pcb_simple,
                             .sched = sched_simple,
                             .init = init_sched_simple,
							 .acquire_lock = acquire_ptable_lock,
							 .release_lock = release_ptable_lock};
struct scheduler simple_scheduler = {.op = &simple_op};

int nextpid = 1;
void swtch(struct context **, struct context *);

static void init_sched_simple() {
    init_spinlock(&ptable.lock, "ptable");
}

static void acquire_ptable_lock() {
    acquire_spinlock(&ptable.lock);
}

static void release_ptable_lock() {
    release_spinlock(&ptable.lock);
}
/*
 * Per-CPU process scheduler
 * Each CPU calls scheduler() after setting itself up.
 * Scheduler never returns.  It loops, doing:
 *  - choose a process to run
 *  - swtch to start running that process
 *  - eventually that process transfers control
 *        via swtch back to the scheduler.
 */
static void scheduler_simple() {
    struct proc *p;
    struct cpu *c = thiscpu();
    c->proc = NULL;

    for (;;) {
        /* Loop over process table looking for process to run. */
        /* TODO: Lab3 Schedule */

    }
}

/*
 * `Swtch` to thiscpu->scheduler.
 */
static void sched_simple() {
    /* TODO: Your code here. */
	if (!holding_spinlock(&ptable.lock)) {
		PANIC("sched: not holding ptable lock");
	}
    if (thiscpu()->proc->state == RUNNING) {
        PANIC("sched: process running");
    }
	/* TODO: Lab3 Schedule */

}

/* 
 * Allocate an unused entry from ptable.
 * Allocate a new pid for it.
 */
static struct proc *alloc_pcb_simple() {
    /* TODO: Lab3 Schedule */

}