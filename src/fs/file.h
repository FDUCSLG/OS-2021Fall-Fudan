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

/* Init File Table Lock. */
void fileinit();

/* 
 * Iterate the file table to get a file structure with ref == 0.
 * Set the ref to 1.
 */
struct file *filealloc();

/*
 * f->ref++
 * ref is to prevent the file from being closed while it is being holded by others
 */
struct file *filedup(struct file *f);

/* 
 * f->ref--
 * if (f->ref == 0) close the file and the inode
 */
void fileclose(struct file *f);

/* 
 * Call stati
 */
int filestat(struct file *f, struct stat *st);

/* 
 * Call inodes.read with range [f->off, f->off + n)
 * Increment f->off by the return value.
 */
isize fileread(struct file *f, char *addr, isize n);

/* 
 * Call inodes.write with range [f->off, f->off + n)
 * Increment f->off by the return value.
 * There should be a maximum valid value of n.
 */
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