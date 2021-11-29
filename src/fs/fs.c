#include <fs/block_device.h>
#include <fs/cache.h>
#include <fs/defines.h>
#include <fs/fs.h>
#include <fs/inode.h>

void init_filesystem() {
    init_block_device();

    const SuperBlock *sblock = get_super_block();
    init_bcache(sblock, &block_device);
    init_inodes(sblock, &bcache);
}
