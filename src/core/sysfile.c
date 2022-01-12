//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#include <fcntl.h>

#include <aarch64/mmu.h>
#include <common/defines.h>
#include <common/spinlock.h>
#include <core/console.h>
#include <core/proc.h>
#include <core/sched.h>
#include <core/sleeplock.h>
#include <fs/file.h>
#include <fs/fs.h>

#include "syscall.h"

struct iovec {
    void *iov_base; /* Starting address. */
    usize iov_len;  /* Number of bytes to transfer. */
};

/*
 * Fetch the nth word-sized system call argument as a file descriptor
 * and return both the descriptor and the corresponding struct file.
 */
static int argfd(int n, i64 *pfd, struct file **pf) {
    i32 fd;
    struct file *f;

    if (argint(n, &fd) < 0)
        return -1;
    if (fd < 0 || fd >= NOFILE || (f = thiscpu()->proc->ofile[fd]) == 0)
        return -1;
    if (pfd)
        *pfd = fd;
    if (pf)
        *pf = f;
    return 0;
}

/*
 * Allocate a file descriptor for the given file.
 * Takes over file reference from caller on success.
 */
static int fdalloc(struct file *f) {
    /* TODO: Lab9 Shell */
    
    return -1;
}

/* 
 * Get the parameters and call filedup.
 */
int sys_dup() {
    /* TODO: Lab9 Shell. */
    return 0;
}

/* 
 * Get the parameters and call fileread.
 */
isize sys_read() {
    /* TODO: Lab9 Shell */
    return -1;
}

/* 
 * Get the parameters and call filewrite.
 */
isize sys_write() {
    /* TODO: Lab9 Shell */
    return -1;
}

isize sys_writev() {
    /* TODO: Lab9 Shell */

    /* Example code.
     *
     * ```
     * struct file *f;
     * i64 fd, iovcnt;
     * struct iovec *iov, *p;
     * if (argfd(0, &fd, &f) < 0 ||
     *     argint(2, &iovcnt) < 0 ||
     *     argptr(1, &iov, iovcnt * sizeof(struct iovec)) < 0) {
     *     return -1;
     * }
     *
     * usize tot = 0;
     * for (p = iov; p < iov + iovcnt; p++) {
     *     // in_user(p, n) checks if va [p, p+n) lies in user address space.
     *     if (!in_user(p->iov_base, p->iov_len))
     *          return -1;
     *     tot += filewrite(f, p->iov_base, p->iov_len);
     * }
     * return tot;
     * ```
     */

    return -1;
}

/* 
 * Get the parameters and call fileclose.
 * Clear this fd of this process.
 */
int sys_close() {
    /* TODO: Lab9 Shell */
    

    return 0;
}

/* 
 * Get the parameters and call filestat.
 */
int sys_fstat() {
    /* TODO: Lab9 Shell */
    return -1;
}

int sys_fstatat() {
    i32 dirfd, flags;
    char *path;
    struct stat *st;

    if (argint(0, &dirfd) < 0 || argstr(1, &path) < 0 || argptr(2, (void *)&st, sizeof(*st)) < 0 ||
        argint(3, &flags) < 0)
        return -1;

    if (dirfd != AT_FDCWD) {
        printf("sys_fstatat: dirfd unimplemented\n");
        return -1;
    }
    if (flags != 0) {
        printf("sys_fstatat: flags unimplemented\n");
        return -1;
    }

    Inode *ip;
    OpContext ctx;
    bcache.begin_op(&ctx);
    if ((ip = namei(path, &ctx)) == 0) {
        bcache.end_op(&ctx);
        return -1;
    }
    inodes.lock(ip);
    stati(ip, st);
    inodes.unlock(ip);
    inodes.put(&ctx, ip);
    bcache.end_op(&ctx);

    return 0;
}

/* 
 * Create an inode.
 *
 * Example:
 * Path is "/foo/bar/bar1", type is normal file.
 * You should get the inode of "/foo/bar", and
 * create an inode named "bar1" in this directory.
 * 
 * If type is directory, you should additionally handle "." and "..".
 */
Inode *create(char *path, short type, short major, short minor, OpContext *ctx) {
    /* TODO: Lab9 Shell */
    return 0;
}

int sys_openat() {
    char *path;
    int dirfd, fd, omode;
    struct file *f;
    Inode *ip;

    if (argint(0, &dirfd) < 0 || argstr(1, &path) < 0 || argint(2, &omode) < 0)
        return -1;

    // printf("%d, %s, %lld\n", dirfd, path, omode);
    if (dirfd != AT_FDCWD) {
        printf("sys_openat: dirfd unimplemented\n");
        return -1;
    }
    // if ((omode & O_LARGEFILE) == 0) {
    //     printf("sys_openat: expect O_LARGEFILE in open flags\n");
    //     return -1;
    // }

    OpContext ctx;
    bcache.begin_op(&ctx);
    if (omode & O_CREAT) {
        // FIXME: Support acl mode.
        ip = create(path, INODE_REGULAR, 0, 0, &ctx);
        if (ip == 0) {
            bcache.end_op(&ctx);
            return -1;
        }
    } else {
        if ((ip = namei(path, &ctx)) == 0) {
            bcache.end_op(&ctx);
            return -1;
        }
        inodes.lock(ip);
        // if (ip->entry.type == INODE_DIRECTORY && omode != (O_RDONLY | O_LARGEFILE)) {
        //     inodes.unlock(ip);
        //     inodes.put(&ctx, ip);
        //     bcache.end_op(&ctx);
        //     return -1;
        // }
    }

    if ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
        if (f)
            fileclose(f);
        inodes.unlock(ip);
        inodes.put(&ctx, ip);
        bcache.end_op(&ctx);
        return -1;
    }
    inodes.unlock(ip);
    bcache.end_op(&ctx);

    f->type = FD_INODE;
    f->ip = ip;
    f->off = 0;
    f->readable = !(omode & O_WRONLY);
    f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
    return fd;
}

int sys_mkdirat() {
    i32 dirfd, mode;
    char *path;
    Inode *ip;

    if (argint(0, &dirfd) < 0 || argstr(1, &path) < 0 || argint(2, &mode) < 0)
        return -1;
    if (dirfd != AT_FDCWD) {
        printf("sys_mkdirat: dirfd unimplemented\n");
        return -1;
    }
    if (mode != 0) {
        printf("sys_mkdirat: mode unimplemented\n");
        return -1;
    }
    OpContext ctx;
    bcache.begin_op(&ctx);
    if ((ip = create(path, INODE_DIRECTORY, 0, 0, &ctx)) == 0) {
        bcache.end_op(&ctx);
        return -1;
    }
    inodes.unlock(ip);
    inodes.put(&ctx, ip);
    bcache.end_op(&ctx);
    return 0;
}

int sys_mknodat() {
    Inode *ip;
    char *path;
    i32 dirfd, major, minor;

    if (argint(0, &dirfd) < 0 || argstr(1, &path) < 0 || argint(2, &major) < 0 || argint(3, &minor))
        return -1;

    if (dirfd != AT_FDCWD) {
        printf("sys_mknodat: dirfd unimplemented\n");
        return -1;
    }
    printf("mknodat: path '%s', major:minor %d:%d\n", path, major, minor);

    OpContext ctx;
    bcache.begin_op(&ctx);
    if ((ip = create(path, INODE_DEVICE, major, minor, &ctx)) == 0) {
        bcache.end_op(&ctx);
        return -1;
    }
    inodes.unlock(ip);
    inodes.put(&ctx, ip);
    bcache.end_op(&ctx);
    return 0;
}

int sys_chdir() {
    char *path;
    Inode *ip;
    struct proc *curproc = thiscpu()->proc;

    OpContext ctx;
    bcache.begin_op(&ctx);
    if (argstr(0, &path) < 0 || (ip = namei(path, &ctx)) == 0) {
        bcache.end_op(&ctx);
        return -1;
    }
    inodes.lock(ip);
    if (ip->entry.type != INODE_DIRECTORY) {
        inodes.unlock(ip);
        inodes.put(&ctx, ip);
        bcache.end_op(&ctx);
        return -1;
    }
    inodes.unlock(ip);
    inodes.put(&ctx, curproc->cwd);
    bcache.end_op(&ctx);
    curproc->cwd = ip;
    return 0;
}
int execve(const char *path, char *const argv[], char *const envp[]);

/* 
 * Get the parameters and call execve.
 */
int sys_exec() {
    /* TODO: Lab9 Shell */
    return -1;
}