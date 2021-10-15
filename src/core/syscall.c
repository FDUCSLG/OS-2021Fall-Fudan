#include <core/console.h>
#include <core/syscall.h>

/*
 * Based on the syscall number, call the corresponding syscall handler.
 * The syscall number and parameters are all stored in the trapframe.
 * See `syscallno.h` for syscall number macros.
 */
u64 syscall_dispatch(Trapframe *frame) {
    /* TODO: Lab3 Syscall */
	
}
