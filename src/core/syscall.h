#pragma once

#include <core/syscallno.h>
#include <core/trapframe.h>

void sys_myexecve(char *s);
NO_RETURN void sys_myexit();
u64 syscall_dispatch(Trapframe *frame);
