#pragma once

#include <core/trapframe.h>
#include <core/syscallno.h>

void sys_myexecve(char *s);
void sys_myexit();
u64 syscall_dispatch(Trapframe *frame);
