# Lab1 启动

本学期，我们将实现一个操作系统内核。第一个实验里，我们的目标是输出 `Hello world!` ，以及了解为什么能输出这一行。

## 1.1 硬件架构 `ARMv8`

> 操作系统抽象并管理底层硬件资源。
>
> 操作系统为软件提供较硬件更为便捷易用的服务。

我们的操作系统内核将运行在 `ARMv8` 硬件架构上。我们已经学过了一些其他的指令集，它们的区别主要在于：

* 指令的表现力，主要分为 `RISC` （精简指令集架构）和 `CISC` （复杂指令集架构）两种。`ARMv8` 和 `MIPS` 一样，都是 `RISC` ；`x86` 是 `CISC`。一般来说，`CISC` 每条指令所执行的功能很多，可支持内存值的算术操作。`RISC` 有专门的访存指令。
* 汇编指令与指令编码。这是最显然的差距，却也是最浅显的。大家只需要了解少数几条常用的汇编，了解在 C 语言里使用 `asm volatile()` 即可。
* 内存映射模型（`Lab2`）。
* 系统寄存器。大家需要重点关注这一部分。与之相关的，包括：
  * 内核态、用户态（`Lab3`）。
  * 异常与中断（`Lab3`）。
  * 进程之间的内存隔离（`Lab2`）。
  * 虚拟化（进阶内容）。
* 内存一致性模型（进阶内容）。
* 等等。

这个学期里，大家要完成的大部分代码都是架构无关代码。涉及到系统寄存器的部分，需要编写汇编代码。

`ARMv8` 指令手册：[Arm Architecture Reference Manual Armv8, for A-profile architecture](https://developer.arm.com/documentation/ddi0487/gb)

## 1.2 实验环境

我们的电脑用的是 `x86` 架构，`gcc` 编译出来的二进制文件不能在 `ARMv8` 的架构上运行。在电脑上安装交叉编译环境，可以解决这个问题。

我们为大家提供了一台服务器，并配好了这个学期实验所需要的所有编译环境。大家可以通过 `ssh` 登录到服务器上，可以在服务器上直接写代码、调试运行内核。详见 [服务器环境手册](./rpi-os-container-handbook)。

在服务器上 clone 代码仓库：

```sh
cd ~/data
git clone git://github.com/FDUCSLG/OS-2021Fall-Fudan

cd OS-2021Fall-Fudan
# 切换到本实验地分支
git checkout lab1

# 新建一个分支，用于开发
git checkout -b lab1-dev

# 测试运行
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make qemu
```

也可以先 fork 一份仓库，再 clone。

我们将使用 `qemu` (quick emulator) 来运行内核。退出方法为： `Ctrl+A`，松开后按 `X`。

## 1.3 启动

> 操作系统如何取得先机？

可以用一段代码来描述“起电”：

```verilog
parameter PCINIT = 64'h80000;
always_ff @(posedge clk) begin
    if (~resetn) pc <= PCINIT;
    else pc <= pc_nxt;
end
```

把操作系统内核的第一条指令安排在 `PCINIT` 位置，操作系统就能在硬件启动后取得先机。

我们用链接器脚本 (linker script) 来实现这一点。链接器脚本可以如下两个问题：

1. 对于编译器而言，例如 `b main` （跳到名为 main 的函数）的指令应该编译成什么？显然，生成对应的机器码需要知道 main 的地址。而链接器脚本就是用来控制这些地址的。
2. 回顾一下树莓派的启动流程，加载内核时是把 kernel8.img 直接复制到物理地址 0x80000 处，然后从 0x80000 开始执行。于是，kernel8.img 的开头处就是我们内核的“入口”（kern/entry.S），换言之，我们需要将 entry.S 放在内核镜像文件的开头，并用 objcopy 去掉 ELF 头。

以下介绍实验里会用到的语法：

- `.` 被称为 location counter，代表了当前位置代码的实际上会运行在内存中的哪个地址。如 `. = 0xFFFF000000080000;` 代表我们告诉链接器内核的第一条指令运行在 0xFFFF000000080000。但事实上，我们一开始是运行在物理地址 0x80000（为什么仍能正常运行？），开启页表后，我们才会跳到高地址（虚拟地址），大部分代码会在那里完成。
- text 段中的第一行为 `KEEP(*(.text.boot))` 是指把 kern/entry.S 中我们自定义的 `.text.boot` 段放在 text 段的开头。`KEEP` 则是告诉链接器始终保留此段。
- `PROVIDE(etext = .)` 声明了一个符号 `etext`（但并不会为其分配具体的内存空间，即在 C 代码中我们不能对其进行赋值），并将其地址设为当前的 location counter。这使得我们可以在汇编或 C 代码中得到内核各段（text/data/bss）的地址范围，我们在初始化 BSS 的时候需要用到这些符号。

本实验用到的链接器脚本是 `src/linker.ld` 。链接器脚本参考：[Linker Scripts](https://sourceware.org/binutils/docs/ld/Scripts.html)

启动代码一般是一些结构相关的配置代码，配置结束后进入 `main` 函数。详见 `src/start.S` 。

以上是一种简化的解决方案。真实的商用化系统不会这么做，原因包括：

* `PCINIT` 没有在架构手册里确定下来，不同硬件可能有不同的 `PCINIT` ，而我们又希望同一内核镜像能在多种硬件上启动。
* 操作系统的文件保存在硬盘上，而内存具有 `volatile` 属性，起电时内存里是没有内容的。需要一段代码把操作系统从硬盘上搬到内存里。

真实系统的启动是十分复杂的过程，本实验不做要求，感兴趣的同学可以和助教讨论。

## 1.4 输出

在用户程序中，`printf` 实现为向 `stdout` 文件描述符写数据。在操作系统里，输出的过程是和某个字符设备进行交互。

本实验采用的交互模型是 `MMIO` (Memory-Mapped I/O) 。 往一个地址写1字节数据，设备就会输出1个字符。详见 `src/core/console.c, src/core/char_device.c, src/driver/uart.c` 。

`qemu` 会发现这段地址属于某个设备，就会把这次写内存操作交给设备驱动代码进行处理。

## 1.5 作业与提交

作业：在 `src/main.c` 中添加代码，要求输出 `Hello world!` 。截止日期为下周上课前（`2021/9/24 15:24:59`）。

提交：将实验报告提交到 elearning 上，格式为`学号-lab1.pdf`。本次实验中，报告不计分。

