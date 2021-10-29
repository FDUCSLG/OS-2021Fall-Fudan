# 实验五：文件系统入门

本次实验你们将实现 xv6 文件系统的中的 inode 层。请将 `src/fs/inode.c` 中的所有 TODO 完成。

参考资料：

1. 实验课 slides
2. The xv6 book，第 8 章，“File system”

建议在动手前先阅读 `src/fs/` 目录下的所有头文件，对整体的代码框架有个大概的了解。

### 测试

本次实验通过标准如下：

1. `make kernel` 编译通过
2. 通过文件系统的单元测试 `inode_test`

运行 `inode_test` 的方法如下：

```shell
cd src/fs/test
mkdir build
cd build
cmake ..
make && ./inode_test
```

如果没有显示任何 `(error)` 或 `(fatal)` 字样，则表明通过测试。

如果遇到问题，请阅读测试代码 `src/fs/test/inode_test.cpp` 并参考实验课 slides 中的调试指南进行调试。

警告：我们不保证测试足够全面，请不要过于依赖于我们提供的测试。如果你的代码存在问题，可能会影响到后续实验。

### 思考题

本部分不计分。

1. 阅读 `src/fs/fs.h` 中的 `InodeEntry` 结构体。我们代码中的一个 inode 能存放多少字节的二进制数据？如果我们想在一个 inode 中存储更多的二进制数据，需要对 inode 的机制和 `InodeEntry` 做怎样的修改？
2. 阅读 `common/defines.h` 中的宏 `offset_of` 和 `container_of`，解释 `container_of` 是如何计算的。
3. 阅读 `src/core/arena.c`。不同的 object 可能由不同的 `Arena` 管理。而释放 object 资源的函数原型是 `void free_object(void *object)`，并没有传入 `Arena` 对象的指针。请说明 `free_object` 是如何找到对应 `Arena` 并释放资源的。
4. 李四和张三用 SSH 同时以 root 用户的身份登录了同一台 Linux 服务器，并处在同一目录下。李四登录后打开了 Python 命令行。此后两人执行了以下操作：

| 时间  |            李四             |       张三        |
| :---: | :-------------------------: | :---------------: |
| 02:08 | `f = open('database', 'w')` |                   |
| 02:09 |      `f.write('nmsl')`      |                   |
| 02:10 |         `f.flush()`         |                   |
| 02:11 |                             |  `cat database`   |
| 02:12 |                             | `rm -rf database` |
| 02:13 |                             |     `logout`      |
| 02:14 |   `f.write('??????????')`   |                   |
| 02:15 |         `f.close()`         |                   |

假设上述所有命令运行时间可以忽略不计，请问：

* 02:12 时张三的命令能否成功？
* 02:14 时 Python 语句能否成功？返回值是多少？
* 用你在 xv6 文件系统中了解的知识，描述 02:15 时操作系统对 `database` 这个文件会有什么操作。
