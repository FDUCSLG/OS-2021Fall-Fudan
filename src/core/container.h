#pragma once

#include <common/spinlock.h>
#include <core/proc.h>
#include <core/sched.h>

#define NPID 64
struct pid_mapping {
    bool valid;
    int pid_local;
    struct proc *p;
};
typedef struct pid_mapping pid_mapping;

typedef enum { MEMORY, PID, INODE } resource_t;

struct container {
    struct proc *p;
    struct scheduler scheduler;
    SpinLock lock;
    struct container *parent;

    // pid
    struct pid_mapping pmap[NPID];
};

typedef struct container container;

extern struct container *root_container;

void init_container();

void *alloc_resource(struct container *this, struct proc *p, resource_t resource);

void trace_usage(struct container *this, struct proc *p, resource_t resource);

void container_test_init();
