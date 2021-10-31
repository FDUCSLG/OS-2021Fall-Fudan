#pragma once

#include <common/defines.h>
#include <fs/log.h>

#define BLOCK_SIZE 512

typedef struct {
    u8 data[BLOCK_SIZE];
} Block;

typedef struct BlockCache {
    // begin a new atomic operation and initialize `ctx`.
    // `OpContext` represents an outstanding atomic operation. You can mark the
    // end of atomic operation by `end_op`.
    void (*begin_op)(OpContext *ctx);

    // end the atomic operation managed by `ctx`.
    // it returns when all associated blocks are synchronized to disk.
    void (*end_op)(OpContext *ctx);

    // allocate a new zero-initialized block.
    // block number is returned.
    usize (*alloc)(OpContext *ctx);

    // mark block at `block_no` is free.
    void (*free)(OpContext *ctx, usize block_no);

    // get and lock the block at `block_no`, incrementing its reference count by one.
    Block *(*acquire)(usize block_no);

    // unlock `block` and decrement its reference count by one, and return it back.
    void (*release)(Block *block);

    // synchronize the content of `block` to disk.
    // `ctx` can be NULL, which indicates this operation does not belong to any
    // atomic operation and it immediately writes all content back to disk. However
    // this is very dangerous, since it may break atomicity of concurrent atomic
    // operations. YOU SHOULD USE THIS MODE WITH CARE.
    // if `ctx` is not NULL, the actual writeback is delayed until `end_op`.
    void (*sync)(OpContext *ctx, Block *block);

    // wait for all outstanding atomic operations to complete and be flushed to disk.
    // it clears all logging and write all dirty blocks back.
    // it is usually used for testing and debugging.
    void (*fence)();
} BlockCache;
