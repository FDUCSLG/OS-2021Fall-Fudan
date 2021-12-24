#pragma once

#include <common/defines.h>
#include <core/sleeplock.h>
#include <fs/defines.h>
#include <fs/fs.h>
#include <fs/inode.h>
#include <sys/stat.h>

#define NFILE 100  // Open files per system

typedef struct file {
    enum { FD_NONE, FD_PIPE, FD_INODE } type;
    int ref;
    char readable;
    char writable;
    struct pipe *pipe;
    Inode *ip;
    usize off;
} File;


void fileinit();
struct file *filealloc();
struct file *filedup(struct file *f);
void fileclose(struct file *f);
int filestat(struct file *f, struct stat *st);
isize fileread(struct file *f, char *addr, isize n);
isize filewrite(struct file *f, char *addr, isize n);

int sys_dup();
isize sys_read();
isize sys_write();
isize sys_writev();
int sys_close();
int sys_fstat();
int sys_fstatat();
int sys_openat();
int sys_mkdirat();
int sys_mknodat();
int sys_chdir();
int sys_exec();