#include <common/string.h>
#include <core/arena.h>
#include <core/container.h>
#include <core/physical_memory.h>
#include <core/sched.h>
#include <core/virtual_memory.h>

struct container *root_container = 0;
static Arena arena;
bool do_cont_test = false;

extern void add_loop_test(int times);

/* 
 * The entry of a new spawned scheduler.
 * Similar to `forkret`.
 * Maintain thiscpu()->scheduler.
 */
static NO_RETURN void container_entry() {
    /* TODO: lab6 container */


	/* container_entry should enter scheduler and should not return */
    PANIC("scheduler should not return");
}

/* 
 * Allocate memory for a container.
 * For root container, a container scheduler is enough. 
 * Memory of struct proc is from another ptable, if root is false.
 * Similar to `alloc_proc`.
 * Initialize some pointers.
 */
struct container *alloc_container(bool root) {
    /* TODO: lab6 container */
	
}

/*
 * Initialize the container system.
 * Initialize the memory pool and root scheduler.
 */
void init_container() {
    /* TODO: lab6 container */

}

/* 
 * Allocating resource should be recorded by each ancestor of the process.
 * You can add parameters if needed.
 */
void *alloc_resource(struct container *this, struct proc *p, resource_t resource) {
    /* TODO: lab6 container */

}

/* 
 * Spawn a new process.
 */
struct container *spawn_container(struct container *this, struct sched_op *op) {
    /* TODO: lab6 container */

}

/*
 * Add containers for test
 */
void container_test_init() {
    struct container *c;

    do_cont_test = true;
    add_loop_test(1);
    c = spawn_container(root_container, &simple_op);
    assert(c != NULL);
}
