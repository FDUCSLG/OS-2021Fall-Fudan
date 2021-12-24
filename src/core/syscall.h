#pragma once

#include <core/syscallno.h>
#include <core/trapframe.h>

void sys_myexecve(char *s);
NO_RETURN void sys_myexit();
void sys_myprint(int x);
u64 syscall_dispatch(Trapframe *frame);

int sys_yield();
usize sys_brk();
int sys_clone();
int sys_wait4();
int sys_exit();
int sys_dup();
isize sys_read();
isize sys_write();
isize sys_writev();
int sys_close();
int sys_fstat();
int sys_fstatat();
Inode *create(char *path, short type, short major, short minor, OpContext *ctx);
int sys_openat();
int sys_mkdirat();
int sys_mknodat();
int sys_chdir();
int sys_exec();
int argint(int n, int *ip);
int argu64(int n, u64 *ip);
int argptr(int n, char **pp, usize size);
int argstr(int n, char **pp);