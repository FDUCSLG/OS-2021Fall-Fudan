#include <stdint.h>

#include <core/proc.h>
#include <core/sched.h>
#include <core/syscall.h>
#include <core/trap.h>

int sys_yield() {
    yield();
    return 0;
}

usize sys_brk() {
    /* TODO: Lab9 Shell */
    return 0;
}

int sys_clone() {
    void *childstk;
    i32 flag;
    if (argint(0, &flag) < 0 || argint(1, &childstk) < 0)
        return -1;
    if (flag != 17) {
        printf("sys_clone: flags other than SIGCHLD are not supported.\n");
        return -1;
    }
    return fork();
}

int sys_wait4() {
    i32 pid, opt;
    int *wstatus;
    void *rusage;
    if (argint(0, &pid) < 0 || argint(1, &wstatus) < 0 || argint(2, &opt) < 0 ||
        argint(3, &rusage) < 0)
        return -1;

    // if (pid != -1 || wstatus != 0 || opt != 0 || rusage != 0) {
    //     printf("sys_wait4: unimplemented. pid %d, wstatus 0x%p, opt 0x%x, rusage 0x%p\n",
    //            pid,
    //            wstatus,
    //            opt,
    //            rusage);
    //     while (1) {}
    //     return -1;
    // }

    return wait();
}

int sys_exit() {
    exit();
}