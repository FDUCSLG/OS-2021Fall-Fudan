# Lab 8：Block Cache

本次实验将实现缓存和日志机制。

参考资料：

1. 实验课 slides
2. The xv6 book，第 8 章，“File system”

你需要实现 `src/fs/cache.c` 中的所有 “`TODO`”。我们建议你在动手实现之前阅读 `src/fs` 目录下的 `cache.h` 及其它头文件中的注释说明。

## 测试

1. `make qemu` 能够正常运行，并且 lab 7 的 `sd_test` 能够通过
2. 通过 `cache_test`：

```shell
$ cd src/fs/test
$ mkdir -p build; cd build
$ cmake ..
$ make && ./cache_test
```

如果遇到编译错误，请尝试运行 `make clean && make`。如果依然不能解决问题，请尝试删除 `build` 文件夹后重试以上步骤。

通过标准：最后一行出现 “`(info) OK: 23 tests passed.`”。此外助教会通过测试的输出判断测试是否在正常运行。

以下是助教的实现的输出：

```plaintext
(info) "init" passed.
(info) "read_write" passed.
(info) "loop_read" passed.
(info) "reuse" passed.
(debug) #cached = 20, #read = 154
(info) "lru" passed.
(info) "atomic_op" passed.
(fatal) assertion failed: "i < OP_MAX_NUM_BLOCKS"
(info) "overflow" passed.
(info) "resident" passed.
(info) "local_absorption" passed.
(info) "global_absorption" passed.
(info) "replay" passed.
(fatal) cache_alloc: no free block
(info) "alloc" passed.
(info) "alloc_free" passed.
(info) "concurrent_acquire" passed.
(info) "concurrent_sync" passed.
(info) "concurrent_alloc" passed.
(info) "simple_crash" passed.
(trace) running: 1000/1000 (844 replayed)
(info) "single" passed.
(trace) running: 1000/1000 (224 replayed)
(info) "parallel_1" passed.
(trace) running: 1000/1000 (168 replayed)
(info) "parallel_2" passed.
(trace) running: 500/500 (221 replayed)
(info) "parallel_3" passed.
(trace) running: 500/500 (229 replayed)
(info) "parallel_4" passed.
(trace) throughput = 20751.62 txn/s
(trace) throughput = 21601.20 txn/s
(trace) throughput = 18314.84 txn/s
(trace) throughput = 21231.88 txn/s
(trace) throughput = 18097.45 txn/s
(trace) throughput = 21225.89 txn/s
(trace) throughput = 18049.48 txn/s
(trace) throughput = 21035.98 txn/s
(trace) throughput = 17995.50 txn/s
(trace) throughput = 21220.89 txn/s
(trace) throughput = 17972.01 txn/s
(trace) throughput = 20875.56 txn/s
(trace) throughput = 18040.48 txn/s
(trace) throughput = 21422.29 txn/s
(trace) throughput = 18128.94 txn/s
(trace) throughput = 21169.92 txn/s
(trace) throughput = 18037.98 txn/s
(trace) throughput = 21279.36 txn/s
(trace) throughput = 18271.36 txn/s
(trace) throughput = 21325.34 txn/s
(trace) throughput = 17884.06 txn/s
(trace) throughput = 21182.41 txn/s
(trace) throughput = 18100.45 txn/s
(trace) throughput = 21194.90 txn/s
(trace) throughput = 18414.29 txn/s
(trace) throughput = 20993.50 txn/s
(trace) throughput = 18106.95 txn/s
(trace) throughput = 21266.87 txn/s
(trace) throughput = 17812.59 txn/s
(trace) throughput = 21317.34 txn/s
(trace) running: 30/30 (7 replayed)
(info) "banker" passed.
(info) OK: 23 tests passed.
```

以上输出无需完全一样。注意中间出现了两个 “`(fatal)`”，是正常现象。

作为参考，助教的实现在服务器上测试耗时 329 秒。

## 调试

Lab 5 中介绍的调试方法在本次实验依然适用。第一种方法是输出调试，这里不多介绍。

第二种方法是 GDB 单步调试。由于 `cache_test` 使用 `fork` 运行单个测试点，因此不方便 GDB 跟进。我们提供了宏 `PAUSE`，你可以在测试点代码中插入 `PAUSE`。当测试运行到 `PAUSE` 处时就会暂停，并且会输出以下信息：

```plaintext
(debug) process 42160 paused.
```

这里 42160 可能会是其它值。之后在另一个 shell 中使用 `gdb -p 42160` 就可以进入测试单步调试。

第三种方法暂停测试。这种方法适用于调试死循环、死锁的情况。当测试陷入死循环、死锁这类问题时，可以按下 `Ctrl+Z` 来中断测试。此时 shell 会暂停测试并将测试进程切换到后台。之后可以使用 `ps` 命令查看测试进程的 PID：

```shell
$ ps
    PID TTY          TIME CMD
  43393 pts/1    00:00:00 fish
  43519 pts/1    00:00:00 cache_test
  43643 pts/1    00:00:00 cache_test
  43672 pts/1    00:00:00 ps
```

可以看到上面有 43519 和 43643 都是 `cache_test`。一般 PID 较大者是实际的在运行的测试。使用 `gdb -p 43643` 就可以进入。进入 GDB 后，使用 `info threads` 命令可以看到每个线程停留在什么函数上。可以使用 `thread [编号]` 来切换当前线程，使用 `backtrace` 来查看当前线程的调用栈。

第四种方法是使用 core dump。这种方法适用于测试异常退出的情况。当测试中的断言失败、内核代码 panic 或者触发段错误时，测试会立刻退出，同时在 `~/data` 目录下产生 core dump 文件：

```shell
$ ls -lh ~/data
total 33M
-rw-------. 1 root root  12M Nov 27 16:48 'core,cache_test,pid=41549,1638002886'
-rw-------. 1 root root  12M Nov 27 16:48 'core,cache_test,pid=41673,1638002886'
-rw-------. 1 root root  12M Nov 27 17:00 'core,cache_test,pid=42614,1638003620'
```

上面的 `core,...` 就是 core dump 文件。文件名的最后一项是时间戳。一般越新的 core dump 文件的时间戳越大。另外，在测试运行时按下 `Ctrl+\`，此时 shell 会发送 `SIGQUIT` 信号，也会导致测试退出并生成 core dump。

可以使用 GDB 打开 core dump 文件：`gdb cache_test 'core,cache_test,pid=42614,1638003620'`。

## 思考题

本部分不计分。

### 一

在我们提供的 `cache.h` 中，`Block` 结构体的定义如下：

```c
typedef struct {
    usize block_no;
    ListNode node;
    bool acquired;
    bool pinned;
    SleepLock lock;
    bool valid;
    u8 data[BLOCK_SIZE];
} Block;
```

1. 在服务器平台上，`Block` 内成员 `data` 在结构体内的偏移量是多少？
2. 已知一个 `Block` 结构体指针 `b` 指向的地址是 `0x1000`。`b` 中的数据是大小为 64 的 `u64` 数组，即 `arr = (u64 *)b->data`。我们一般希望访问 `u64` 变量时的地址和 8 字节对齐，请问 `arr` 是否满足了字节对齐的要求？如果没有，如何解决这个问题？
3. 我们使用通用对象内存池 `Arena` 来分配 `Block` 结构体。请问 `Arena` 是否会影响 `u64` 的字节对齐？
4. 一些 `memcpy` 的实现会使用 SIMD。为了达到最高的效率，它们可能希望地址和 64 字节对齐。请问此时 `Arena` 是否会有影响？如果有影响，请设计一个方案，确保块数据（`b->data`）地址都和 64 字节对齐。
    * 提示：你可以重构 `Block` 和 `Arena`。

### 二

在 xv6 的设计中，进行原子操作写回硬盘（`checkpoint`）的同时，新的 `begin_op` 都将被阻塞，直到 `checkpoint` 完成。张三想优化这一点。

1. 张三希望，在 `checkpoint` 时，允许有 `begin_op` 创建新的原子操作，并进行读写。不过这些原子操作的 `end_op` 须等待之前的 `checkpoint` 完成。此时依然存在一个问题：如果新的原子操作 A 写了块 B，并且块 B 出现在当前 `checkpoint` 的日志中，那么块 B 的新数据有可能在 A 的 `end_op` 之前，被 `checkpoint` 提前写回到硬盘，破坏了 A 的原子性。请问该如何解决这个问题？
2. 为了进一步缓解 `begin_op`/`end_op` 被 `checkpoint` 阻塞的情况，张三在 `checkpoint` 之前，将所有记录在日志中的块做一份内存中的 copy，这样就避免了 `checkpoint` 和后续原子操作读写之间的数据竞争。内存复制的速度远快于硬盘读写速度，因此张三十分满意。请问这个方法是否还有优化空间？

### 三

现代的存储设备一般支持并行读写。一种提高读写并行度的方法是向量化读写（vector read/write），允许一次性提交若干读写请求，并等待所有请求完成后再返回。同一次向量读写中的请求可能会乱序完成。在 xv6 的设计中，一次 `checkpoint` 可以分成 4 次 vector write：

* 将所有块写入到日志区域
* 设置 `LogHeader.num_blocks`，并写回硬盘。此时标志所有原子操作已经提交
* 将所有块写到其在硬盘上原本的位置
* 把 `LogHeader.num_blocks` 置零，并写回硬盘

张三又想来做优化。

1. 张三觉得前两步可以直接放在同一个 vector write 中。这样做会有问题吗？
2. 李四觉得在 `LogHeader` 中多记录一些信息，可以只用一个 vector write 完成前两步。请思考该如何设计这一机制。
