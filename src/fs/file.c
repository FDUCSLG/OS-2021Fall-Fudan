/* File descriptors */

#include "file.h"
#include "fs.h"
#include <common/defines.h>
#include <common/spinlock.h>
#include <core/console.h>
#include <core/sleeplock.h>
#include <fs/inode.h>

// struct devsw devsw[NDEV];
struct {
    struct SpinLock lock;
    struct file file[NFILE];
} ftable;

/* Optional since BSS is zero-initialized. */
void fileinit() {
    init_spinlock(&ftable.lock, "file table");
}

/* Allocate a file structure. */
struct file *filealloc() {
    /* TODO: Lab9 Shell */
    return 0;
}

/* Increment ref count for file f. */
struct file *filedup(struct file *f) {
    /* TODO: Lab9 Shell */

    return f;
}

/* Close file f. (Decrement ref count, close when reaches 0.) */
void fileclose(struct file *f) {
    /* TODO: Lab9 Shell */
}

/* Get metadata about file f. */
int filestat(struct file *f, struct stat *st) {
    /* TODO: Lab9 Shell */
}

/* Read from file f. */
isize fileread(struct file *f, char *addr, isize n) {
    /* TODO: Lab9 Shell */
}

/* Write to file f. */
isize filewrite(struct file *f, char *addr, isize n) {
    /* TODO: Lab9 Shell */
}
