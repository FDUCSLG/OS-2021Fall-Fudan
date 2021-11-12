#include <core/console.h>
#include <core/cpu.h>
#include <core/proc.h>
#include <core/syscall.h>

void sys_myexecve(char *s) {
    printf("sys_exec: executing %s\n", s);
    return;
}

NO_RETURN void sys_myexit() {
    printf("sys_exit: in exit\n");
    exit();
}

/* myprint(int x) =>
            printf("pid %d, pid in root %d, cnt %d\n", getpid(), getrootpid(), x);
            yield(); */
void sys_myprint(int x) {
    int rootpid = -1;
    for (int i = 0; i < NPID; i++) {
        if (root_container->pmap[i].valid && root_container->pmap[i].p == thiscpu()->proc)
            rootpid = root_container->pmap[i].pid_local;
    }
    assert(rootpid > 0);
    printf("pid %d, pid in root %d, cnt %d\n", thiscpu()->proc->pid, rootpid, x);
    yield();
}
