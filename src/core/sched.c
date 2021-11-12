#include <common/defines.h>
#include <core/console.h>
#include <core/container.h>
#include <core/sched.h>
#include <core/virtual_memory.h>

#ifdef MULTI_SCHEDULER

/* TODO: Lab6 Add more Scheduler Policies */
static void scheduler_simple(struct scheduler *this);
static struct proc *alloc_pcb_simple(struct scheduler *this);
static void sched_simple(struct scheduler *this);
static void init_sched_simple(struct scheduler *this);
static void acquire_ptable_lock(struct scheduler *this);
static void release_ptable_lock(struct scheduler *this);
struct sched_op simple_op = {.scheduler = scheduler_simple,
                             .alloc_pcb = alloc_pcb_simple,
                             .sched = sched_simple,
                             .init = init_sched_simple,
                             .acquire_lock = acquire_ptable_lock,
                             .release_lock = release_ptable_lock};
struct scheduler simple_scheduler = {.op = &simple_op};

void swtch(struct context **, struct context *);

static void init_sched_simple(struct scheduler *this) {
    init_spinlock(&this->ptable.lock, "ptable");
}

static void acquire_ptable_lock(struct scheduler *this) {
    acquire_spinlock(&this->ptable.lock);
}

static void release_ptable_lock(struct scheduler *this) {
    release_spinlock(&this->ptable.lock);
}

/* 
 * Scheduler yields to its parent scheduler.
 * If this == root, just return.
 * Pay attention to thiscpu() structure and locks.
 */
void yield_scheduler(struct scheduler *this) {

}

NO_RETURN void scheduler_simple(struct scheduler *this) {
    
}

static void sched_simple(struct scheduler *this) {
    
}

static struct proc *alloc_pcb_simple(struct scheduler *this) {
    
}

#endif
