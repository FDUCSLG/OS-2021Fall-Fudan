#pragma once

#include <core/syscallno.h>
#include <core/trapframe.h>

void sys_myexecve(char *s);
NO_RETURN void sys_myexit();
void sys_myprint(int x);
u64 syscall_dispatch(Trapframe *frame);
