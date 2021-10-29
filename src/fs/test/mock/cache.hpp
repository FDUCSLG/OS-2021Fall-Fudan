#pragma once

extern "C" {
#include <fs/inode.h>
}

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <random>
#include <unordered_map>

#include "../exception.hpp"

struct MockBlockCache {
    static constexpr usize num_blocks = 2000;
    static constexpr usize inode_start = 200;
    static constexpr usize block_start = 1000;
    static constexpr usize num_inodes = 1000;

    static auto get_sblock() -> SuperBlock {
        SuperBlock sblock;
        sblock.num_blocks = num_blocks;
        sblock.num_data_blocks = num_blocks - block_start;
        sblock.num_inodes = num_inodes;
        sblock.num_log_blocks = 50;
        sblock.log_start = 100;
        sblock.inode_start = inode_start;
        sblock.bitmap_start = 2;
        return sblock;
    }

    struct Meta {
        bool mark = false;
        std::mutex mutex;
        bool used;

        auto operator=(const Meta &rhs) -> Meta & {
            used = rhs.used;
            return *this;
        }
    };

    struct Cell {
        bool mark = false;
        usize index;
        std::mutex mutex;
        Block block;

        auto operator=(const Cell &rhs) -> Cell & {
            block = rhs.block;
            return *this;
        }

        void zero() {
            for (usize i = 0; i < BLOCK_SIZE; i++) {
                block.data[i] = 0;
            }
        }

        void random(std::mt19937 &gen) {
            for (usize i = 0; i < BLOCK_SIZE; i++) {
                block.data[i] = gen() & 0xff;
            }
        }
    };

    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<usize> oracle, top;
    std::unordered_map<usize, bool> board;
    Meta tmpv[num_blocks], memv[num_blocks];
    Cell tmp[num_blocks], mem[num_blocks];

    MockBlockCache() {
        std::mt19937 gen(0x19260817);

        oracle.store(1);
        top.store(0);

        // fill disk with junk.
        for (usize i = 0; i < num_blocks; i++) {
            tmpv[i].used = false;
            tmp[i].index = i;
            tmp[i].random(gen);
            memv[i].used = false;
            mem[i].index = i;
            mem[i].random(gen);
        }

        // mock superblock.
        auto sblock = get_sblock();
        u8 *buf = reinterpret_cast<u8 *>(&sblock);
        for (usize i = 0; i < sizeof(sblock); i++) {
            mem[1].block.data[i] = buf[i];
        }

        // mock inodes.
        InodeEntry node[num_inodes];
        for (usize i = 0; i < num_inodes; i++) {
            node[i].type = INODE_INVALID;
            node[i].major = gen() & 0xffff;
            node[i].minor = gen() & 0xffff;
            node[i].num_links = gen() & 0xffff;
            node[i].num_bytes = gen() & 0xffff;
            for (usize j = 0; j < INODE_NUM_DIRECT; j++) {
                node[i].addrs[j] = gen();
            }
            node[i].indirect = gen();
        }

        // mock root inode.
        node[1].type = INODE_DIRECTORY;
        node[1].major = 0;
        node[1].minor = 0;
        node[1].num_links = 1;
        node[1].num_bytes = 0;
        for (usize i = 0; i < INODE_NUM_DIRECT; i++) {
            node[1].addrs[i] = 0;
        }
        node[1].indirect = 0;

        usize step = 0;
        for (usize i = 0, j = inode_start; i < num_inodes; i += step, j++) {
            step = std::min(num_inodes - i, static_cast<usize>(INODE_PER_BLOCK));
            buf = reinterpret_cast<u8 *>(&node[i]);
            for (usize k = 0; k < step * sizeof(InodeEntry); k++) {
                mem[j].block.data[k] = buf[k];
            }
        }
    }

    // invalidate all cached blocks and fill them with random data.
    void fill_junk() {
        std::mt19937 gen(0xdeadbeef);

        for (usize i = 0; i < num_blocks; i++) {
            std::scoped_lock guard(tmpv[i].mutex);
            if (tmpv[i].mark)
                throw Internal("marked by others");
        }

        for (usize i = 0; i < num_blocks; i++) {
            std::scoped_lock guard(tmp[i].mutex);
            if (tmp[i].mark)
                throw Internal("marked by others");
            tmp[i].random(gen);
        }
    }

    // count how many inodes on disk are valid.
    auto count_inodes() -> usize {
        std::unique_lock lock(mutex);

        usize step = 0, count = 0;
        for (usize i = 0, j = inode_start; i < num_inodes; i += step, j++) {
            step = std::min(num_inodes - i, static_cast<usize>(INODE_PER_BLOCK));
            auto *inodes = reinterpret_cast<InodeEntry *>(mem[j].block.data);
            for (usize k = 0; k < step; k++) {
                if (inodes[k].type != INODE_INVALID)
                    count++;
            }
        }

        return count;
    }

    // count how many blocks on disk are allocated.
    auto count_blocks() -> usize {
        std::unique_lock lock(mutex);

        usize count = 0;
        for (usize i = block_start; i < num_blocks; i++) {
            std::scoped_lock guard(memv[i].mutex);
            if (memv[i].used)
                count++;
        }

        return count;
    }

    // inspect on disk inode at specified inode number.
    auto inspect(usize i) -> InodeEntry * {
        usize j = inode_start + i / INODE_PER_BLOCK;
        usize k = i % INODE_PER_BLOCK;
        auto *arr = reinterpret_cast<InodeEntry *>(mem[j].block.data);
        return &arr[k];
    }

    void check_block_no(usize i) {
        if (i >= num_blocks)
            throw Panic("block number out of range");
    }

    auto check_and_get_cell(Block *b) -> Cell * {
        Cell *p = container_of(b, Cell, block);
        isize offset = reinterpret_cast<u8 *>(p) - reinterpret_cast<u8 *>(tmp);
        if (offset % sizeof(Cell) != 0)
            throw Panic("pointer not aligned");

        isize i = p - tmp;
        if (i < 0 || static_cast<usize>(i) >= num_blocks)
            throw Panic("block is not managed by cache");

        return p;
    }

    template <typename T>
    void load(T &a, T &b) {
        if (!a.mark) {
            a = b;
            a.mark = true;
        }
    }

    template <typename T>
    void store(T &a, T &b) {
        if (a.mark) {
            b = a;
            a.mark = false;
        }
    }

    void begin_op(OpContext *ctx) {
        std::unique_lock lock(mutex);
        ctx->id = oracle.fetch_add(1);
        board[ctx->id] = false;
    }

    void end_op(OpContext *ctx) {
        std::unique_lock lock(mutex);
        board[ctx->id] = true;

        bool do_checkpoint = true;
        for (const auto &e : board) {
            do_checkpoint &= e.second;
        }

        if (do_checkpoint) {
            for (usize i = 0; i < num_blocks; i++) {
                std::scoped_lock guard(tmpv[i].mutex, memv[i].mutex);
                store(tmpv[i], memv[i]);
            }

            for (usize i = 0; i < num_blocks; i++) {
                std::scoped_lock guard(tmp[i].mutex, mem[i].mutex);
                store(tmp[i], mem[i]);
            }

            usize max_oracle = 0;
            for (const auto &e : board) {
                max_oracle = std::max(max_oracle, e.first);
            }
            top.store(max_oracle);
            board.clear();

            cv.notify_all();
        } else
            cv.wait(lock, [&] { return ctx->id <= top.load(); });
    }

    auto alloc(OpContext *ctx) -> usize {
        for (usize i = block_start; i < num_blocks; i++) {
            std::scoped_lock guard(tmpv[i].mutex, memv[i].mutex);
            load(tmpv[i], memv[i]);

            if (!tmpv[i].used) {
                tmpv[i].used = true;
                if (!ctx)
                    store(tmpv[i], memv[i]);

                std::scoped_lock guard(tmp[i].mutex, mem[i].mutex);
                load(tmp[i], mem[i]);
                tmp[i].zero();
                if (!ctx)
                    store(tmp[i], mem[i]);

                return i;
            }
        }

        throw Panic("no free block");
    }

    void free(OpContext *ctx, usize i) {
        check_block_no(i);

        std::scoped_lock guard(tmpv[i].mutex, memv[i].mutex);
        load(tmpv[i], memv[i]);
        if (!tmpv[i].used)
            throw Panic("free unused block");

        tmpv[i].used = false;
        if (!ctx)
            store(tmpv[i], memv[i]);
    }

    auto acquire(usize i) -> Block * {
        check_block_no(i);

        tmp[i].mutex.lock();

        {
            std::scoped_lock guard(mem[i].mutex);
            load(tmp[i], mem[i]);
        }

        return &tmp[i].block;
    }

    void release(Block *b) {
        auto *p = check_and_get_cell(b);
        p->mutex.unlock();
    }

    void sync(OpContext *ctx, Block *b) {
        auto *p = check_and_get_cell(b);
        usize i = p->index;

        if (!ctx) {
            std::scoped_lock guard(mem[i].mutex);
            store(tmp[i], mem[i]);
        }
    }

    void fence() {
        std::unique_lock lock(mutex);
        usize current = oracle.load() - 1;
        cv.wait(lock, [&] { return current <= top.load(); });
    }
};

namespace {
#include "cache.ipp"
}  // namespace
