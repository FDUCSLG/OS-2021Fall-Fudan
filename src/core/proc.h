#pragma once
#include <common/defines.h>
// #include <core/sched.h>
#include <core/trapframe.h>


#define NPROC      64   /* maximum number of processes */
// #define NOFILE     16   /* open files per process */
#define KSTACKSIZE 4096 /* size of per-process kernel stack */

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

/* 
 * Context should at least contain callee-saved registers.
 * You can add more information in it.
 */
/* Stack must always be 16 bytes aligned. */
struct context {
    /* TODO: Lab3 Process */

};

struct proc {
    u64 sz;             /* Size of process memory (bytes)          */
    u64 *pgdir;         /* Page table                              */
    char *kstack;            /* Bottom of kernel stack for this process */
    enum procstate state;    /* Process state                           */
    int pid;                 /* Process ID                              */
    struct proc *parent;     /* Parent process                          */
    Trapframe *tf;    /* Trapframe for current syscall           */
    struct context *context; /* swtch() here to run process             */
    void *chan;              /* If non-zero, sleeping on chan           */
    int killed;              /* If non-zero, have been killed           */
    char name[16];           /* Process name (debugging)                */
};
typedef struct proc proc;
void init_proc();
void spawn_init_process();
void yield();
void exit();