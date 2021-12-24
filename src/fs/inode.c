#include <common/string.h>
#include <core/arena.h>
#include <core/console.h>
#include <core/physical_memory.h>
#include <core/proc.h>
#include <core/sched.h>
#include <fs/inode.h>

// this lock mainly prevents concurrent access to inode list `head`, reference
// count increment and decrement.
static SpinLock lock;
static ListNode head;

static const SuperBlock *sblock;
static const BlockCache *cache;
static Arena arena;

// return which block `inode_no` lives on.
static INLINE usize to_block_no(usize inode_no) {
    return sblock->inode_start + (inode_no / (INODE_PER_BLOCK));
}

// return the pointer to on-disk inode.
static INLINE InodeEntry *get_entry(Block *block, usize inode_no) {
    return ((InodeEntry *)block->data) + (inode_no % INODE_PER_BLOCK);
}

// return address array in indirect block.
static INLINE u32 *get_addrs(Block *block) {
    return ((IndirectBlock *)block->data)->addrs;
}

// initialize inode tree.
void init_inodes(const SuperBlock *_sblock, const BlockCache *_cache) {
    ArenaPageAllocator allocator = {.allocate = kalloc, .free = kfree};

    init_spinlock(&lock, "inode tree");
    init_list_node(&head);
    sblock = _sblock;
    cache = _cache;
    init_arena(&arena, sizeof(Inode), allocator);

    if (ROOT_INODE_NO < sblock->num_inodes)
        inodes.root = inodes.get(ROOT_INODE_NO);
    else
        printf("(warn) init_inodes: no root inode.\n");
}

// initialize in-memory inode.
static void init_inode(Inode *inode) {
    init_spinlock(&inode->lock, "inode");
    init_rc(&inode->rc);
    init_list_node(&inode->node);
    inode->inode_no = 0;
    inode->valid = false;
}

// see `inode.h`.
static usize inode_alloc(OpContext *ctx, InodeType type) {
    assert(type != INODE_INVALID);

    // TODO

    PANIC("failed to allocate inode on disk");
}

// see `inode.h`.
static void inode_lock(Inode *inode) {
    assert(inode->rc.count > 0);
    acquire_spinlock(&inode->lock);
}

// see `inode.h`.
static void inode_unlock(Inode *inode) {
    assert(holding_spinlock(&inode->lock));
    assert(inode->rc.count > 0);
    release_spinlock(&inode->lock);
}

// see `inode.h`.
static void inode_sync(OpContext *ctx, Inode *inode, bool do_write) {
    // TODO
}

// see `inode.h`.
static Inode *inode_get(usize inode_no) {
    assert(inode_no > 0);
    assert(inode_no < sblock->num_inodes);

    // TODO

    return NULL;
}

// see `inode.h`.
static void inode_clear(OpContext *ctx, Inode *inode) {
    InodeEntry *entry = &inode->entry;

    // TODO
}

// see `inode.h`.
static Inode *inode_share(Inode *inode) {
    acquire_spinlock(&lock);
    increment_rc(&inode->rc);
    release_spinlock(&lock);
    return inode;
}

// see `inode.h`.
static void inode_put(OpContext *ctx, Inode *inode) {
    // TODO
}

// this function is private to inode layer, because it can allocate block
// at arbitrary offset, which breaks the usual file abstraction.
//
// retrieve the block in `inode` where offset lives. If the block is not
// allocated, `inode_map` will allocate a new block and update `inode`, at
// which time, `*modified` will be set to true.
// the block number is returned.
//
// NOTE: caller must hold the lock of `inode`.
static usize inode_map(OpContext *ctx, Inode *inode, usize offset, bool *modified) {
    InodeEntry *entry = &inode->entry;

    // TODO

    return 0;
}

// see `inode.h`.
static usize inode_read(Inode *inode, u8 *dest, usize offset, usize count) {
    InodeEntry *entry = &inode->entry;

    if (inode->entry.type == INODE_DEVICE) {
        assert(inode->entry.major == 1);
        return console_read(inode, dest, count);
    }
    if (count + offset > entry->num_bytes)
        count = entry->num_bytes - offset;
    usize end = offset + count;
    assert(offset <= entry->num_bytes);

    assert(end <= entry->num_bytes);
    assert(offset <= end);

    // TODO
    return count;
}

// see `inode.h`.
static usize inode_write(OpContext *ctx, Inode *inode, u8 *src, usize offset, usize count) {
    InodeEntry *entry = &inode->entry;
    usize end = offset + count;
    if (inode->entry.type == INODE_DEVICE) {
        assert(inode->entry.major == 1);
        return console_write(inode, src, count);
    }
    assert(offset <= entry->num_bytes);
    assert(end <= INODE_MAX_BYTES);
    assert(offset <= end);

    // TODO
    return count;
}

// see `inode.h`.
static usize inode_lookup(Inode *inode, const char *name, usize *index) {
    InodeEntry *entry = &inode->entry;
    assert(entry->type == INODE_DIRECTORY);

    // TODO

    return 0;
}

// see `inode.h`.
static usize inode_insert(OpContext *ctx, Inode *inode, const char *name, usize inode_no) {
    InodeEntry *entry = &inode->entry;
    assert(entry->type == INODE_DIRECTORY);

    // TODO

    return 0;
}

// see `inode.h`.
static void inode_remove(OpContext *ctx, Inode *inode, usize index) {
    InodeEntry *entry = &inode->entry;
    assert(entry->type == INODE_DIRECTORY);

    // TODO
}

/* Paths. */

/* Copy the next path element from path into name.
 *
 * Return a pointer to the element following the copied one.
 * The returned path has no leading slashes,
 * so the caller can check *path=='\0' to see if the name is the last one.
 * If no name to remove, return 0.
 *
 * Examples:
 *   skipelem("a/bb/c", name) = "bb/c", setting name = "a"
 *   skipelem("///a//bb", name) = "bb", setting name = "a"
 *   skipelem("a", name) = "", setting name = "a"
 *   skipelem("", name) = skipelem("////", name) = 0
 */
static const char *skipelem(const char *path, char *name) {
    const char *s;
    int len;

    while (*path == '/')
        path++;
    if (*path == 0)
        return 0;
    s = path;
    while (*path != '/' && *path != 0)
        path++;
    len = path - s;
    if (len >= FILE_NAME_MAX_LENGTH)
        memmove(name, s, FILE_NAME_MAX_LENGTH);
    else {
        memmove(name, s, len);
        name[len] = 0;
    }
    while (*path == '/')
        path++;
    return path;
}

/* Look up and return the inode for a path name.
 * 
 * If parent != 0, return the inode for the parent and copy the final
 * path element into name, which must have room for DIRSIZ bytes.
 * Must be called inside a transaction since it calls iput().
 */
static Inode *namex(const char *path, int nameiparent, char *name, OpContext *ctx) {
	/* TODO: Lab9 Shell */
	
    return 0;
}

Inode *namei(const char *path, OpContext *ctx) {
    char name[FILE_NAME_MAX_LENGTH];
    return namex(path, 0, name, ctx);
}

Inode *nameiparent(const char *path, char *name, OpContext *ctx) {
    return namex(path, 1, name, ctx);
}

/*
 * Copy stat information from inode.
 * Caller must hold ip->lock.
 */
void stati(Inode *ip, struct stat *st) {
    // TODO: Lab9 Shell
    st->st_dev = 1;

    switch (ip->entry.type) {
        case INODE_REGULAR: st->st_mode = S_IFREG; break;
        case INODE_DIRECTORY: st->st_mode = S_IFDIR; break;
        case INODE_DEVICE: st->st_mode = 0; break;
        default: PANIC("unexpected stat type %d. ", ip->entry.type);
    }
}
InodeTree inodes = {
    .alloc = inode_alloc,
    .lock = inode_lock,
    .unlock = inode_unlock,
    .sync = inode_sync,
    .get = inode_get,
    .clear = inode_clear,
    .share = inode_share,
    .put = inode_put,
    .read = inode_read,
    .write = inode_write,
    .lookup = inode_lookup,
    .insert = inode_insert,
    .remove = inode_remove,
};
