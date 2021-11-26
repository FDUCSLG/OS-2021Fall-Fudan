#pragma once

#include <common/defines.h>
#include <common/list.h>
#include <common/string.h>

#define BSIZE 512

#define B_VALID 0x2 /* Buffer has been read from disk. */
#define B_DIRTY 0x4 /* Buffer needs to be written to disk. */

struct buf {
    int flags;
    u32 blockno;
    u8 data[BSIZE];  // 1B*512

    /* 
     * Add other necessary elements. It depends on you.
     */
    /* TODO: Lab7 driver. */

};

/* 
 * Add some useful functions to use your buffer list, such as push, pop and so on.
 */

/* TODO: Lab7 driver. */