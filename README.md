# CacheLab2

RUC CacheLab 2 - CMU CacheLab 升级版

## 如何编译本项目

```shell
make -j
```

## 问题背景

2022年的冬天，小民在写完 CacheLab 后直呼，这作业也太简单了，网上随便抄抄就满分了。

转眼来到了 2024 年，为了尽量避免小明这样的漏网之鱼，RUC CacheLab 2 诞生了。

本实验基于以下现实问题：

CPU 在工作时不能直接用内存中的数据计算，而是通过一定的层次结构层层传递到 CPU 内部再传出，具体来说，内存中的值会先加载到 cache 里，再从 cache 加载到寄存器 register 里，CPU 用寄存器完成计算后把计算结果写回 cache，再写回内存。

所以我们有至少两种优化思路：

- 减少内存读写：通过访问模式设计，减少 cache miss
- 减少 cache 读写：通过访问模式设计，尽量复用寄存器，减少去读写 cache，为了进一步让你们理解，你们可以仔细阅读 [缓存和寄存器性能](#缓存和寄存器性能)

## 问题简述与提示

本实验共有两个部分。在第一部分 Part A 中，你需要实现一个 cache simulator，用来统计内存缓存的命中次数等（不涉及寄存器）。第二部分，你需要实现一个矩阵乘法算子，并最优化其缓存和寄存器性能。

### 缓存和寄存器性能

缓存性能顾名思义，就是减少 cache miss 的次数。

与以往 CacheLab 不同的是，本实验显式地加入了对寄存器性能的考量，让我们用以下例子说明：

考虑两个 $2 \times 2$ 矩阵相乘的场景，假设此时所有数据都在 cache 里，我们只关注寄存器层面的问题。

```c
for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
        for (int k = 0; k < 2; k++) {
            C[i][j] += A[i][k] * B[k][j];
        }
    }
}
```

这是一个典型的矩阵乘法代码，如果直接执行，它的汇编指令类似这样：

```
load A[0][0] to reg1
load B[0][0] to reg2
    calculate reg3 = reg1 * reg2
load C[0][0] to reg4
    calculate reg4 = reg4 + reg3
store reg4 to C[0][0]
...
```

一共需要执行 8 次这样的操作，意味着 24 次 load 和 8 次 store。

为了优化，我们显式地声明寄存器来规定代码的行为。

```c
for (reg i = 0; i < 2; ++i) {
    for (reg j = 0; j < 2; ++j) {
        reg tmpc = 0;
        for (reg k = 0; k < 2; ++k) {
            reg tmpa = A[i][k];
            reg tmpb = B[k][j];
            tmpc += tmpa * tmpb;
        }
        C[i][j] = tmpc;
    }
}
```

此时经过优化后的汇编指令类似这样：

```c
    calculate tmpc = 0
load A[0][0] to tmpa
load B[0][0] to tmpb
    calculate tmpa = tmpa * tmpb
    calculate tmpc = tmpc + tmpa
store tmpc to C[0][0]
...
```

这样的操作执行 8 遍，最后只需要执行 16 次 load 和 4 次 store，减少了大量的 cache 访问。

我们还可以优化，注意到数据实际上只有 8 个 int 要读入，我们却使用了 16 次 load，一个暴力的优化方法是，全部加载到寄存器，计算后再导出：

```c
reg tmpa[2][2];
reg tmpb[2][2];
reg tmpc[2][2];
for (reg i = 0; i < 2; ++i) {
    for (reg j = 0; j < 2; ++j) {
        tmpc[i][j] = 0;
        tmpa[i][j] = A[i][j];
        tmpb[i][j] = B[i][j];
    }
}
for (reg i = 0; i < 2; ++i) {
    for (reg j = 0; j < 2; ++j) {
        for (reg k = 0; k < 2; ++k) {
            tmpc[i][j] += tmpa[i][k] * tmpb[k][j];
        }
    }
}
for (reg i = 0; i < 2; ++i) {
    for (reg j = 0; j < 2; ++j) {
        C[i][j] = tmpc[i][j];
    }
}
```

这样我们达到了理论上的最优性能，只需要 8 次 load 和 4 次 store。代价是我们需要大量的寄存器，而 CPU 上的寄存器是有限的，我们会在问题描述中详细规定硬件配置。

## 问题描述

### Part A：缓存模拟器

我们提供了生成内存访问记录的框架，

比如运行朴素实现的 2*2*2 矩阵乘法：

```
./main case0
```

它会输出 trace 文件，形如：

```
 L 30000000,4 8
 L 30000004,4 9
 L 30000001,4 8
 L 30000006,4 9
 S 30000008,4 6
```

其中 L 和 S 分别代表 load 和 store，后面的数字代表地址，4 代表访问的内存大小（byte记），最后一个数字代表的是与这行操作相关的寄存器（序号其实不影响你的任务），这个数字为 -1 时代表立即数，比如 `C[0] = 0`。

这一部分你需要做的是补全 `csim.c` 文件，使其能够正确地模拟 LRU 替换策略的 cache 的行为，输出 cache 的命中次数（hits）、缺失次数（misses）、替换次数（evictions）。cache 的硬件配置通过命令行参数给出，解析命令行参数已经写完了，你们只需要补全 cache 本身的代码。

命令行用法如下：

```
Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>
Options:
  -h         Print this help message.
  -v         Optional verbose flag.
  -s <num>   Number of set index bits.
  -E <num>   Number of lines per set.
  -b <num>   Number of block offset bits.
  -t <file>  Trace file.

Examples:
  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace
  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace
```

完成后，我们会将你的 csim 和助教提供的 csim-ref 的运行结果进行比对，测试文件在 `traces` 文件夹下，你需要保证输出一致。

我们默认启动 O2 优化，如果这对你的调试产生了干扰，你可以修改 Makefile 中的 `CSIMFALGS`。

#### 实现要求

- 你需要保证你的代码支持任意合理的 s，E，b 参数
- 你需要保证你的代码没有任何编译警告

### Part B：矩阵乘法优化

我们在 [缓存和寄存器性能](#缓存和寄存器性能) 中提到了如何优化矩阵乘法的性能，现在你需要结合 cache 和寄存器优化，基于我们提供的框架，将这些优化实现进 `gemm.cpp` 里，目前 `gemm.cpp` 只提供了最朴素的算法用作演示。

我们测试的环境为

- cache：LRU 替换策略，s = 5，E = 1，b = 5
- CPU：一共 32 个寄存器，我们的框架会在你使用超过 32 个寄存器时抛出运行时错误

你们需要优化的只有 case1，case2，case3 三种情况，分别对应

- case1：$32 \times 32$ 矩阵乘法
- case2：$64 \times 64$ 矩阵乘法
- case3：$61 \times 67$ 矩阵乘法

#### 框架说明

寄存器通常是汇编层面的代码才可以控制的，为了避免各位同学直接手搓汇编，我们提供了一个框架，其中定义了两种新的类型

- `dtype_ptr`：内存指针类型，你可以视作 `int*`，其会占用一个寄存器，支持绝大多数指针类型支持的操作，但是会在你们尝试直接操作内存时报错，比如你不能写 `A[0] * B[0]`，因为 CPU 不能直接访问内存，你需要先存到寄存器里。
- `reg`：寄存器类型，你可以视作 `int`，其会占用一个寄存器，支持绝大多数 int 支持的操作。

我们要求你们在 `gemm.cpp` 中只使用这两种类型，不出现任何的 `int` 之类的类型。

比如

```cpp
void gemm_case1(dtype_ptr A, dtype_ptr B, dtype_ptr C, dtype_ptr buffer) {
    for (reg i = 0; i < m; ++i) {
        for (reg j = 0; j < p; ++j) {
            reg tmpc = 0;
            for (reg k = 0; k < n; ++k) {
                reg tmpa = A[i * n + k];
                reg tmpb = B[k * p + j];
                tmpc += tmpa * tmpb;
            }
            C[i * p + j] = tmpc;
        }
    }
}
```

注意，我们提供了一段 `buffer` 内存，该段内存的大小为 256 byte。你们可以使用这段内存来存储一些中间结果，作为我们要求中不允许自己申请内存的补偿，但是请注意，这段内存是有限的，而且也会产生 cache miss，你们需要合理利用。

更多关于框架的说明请参考 [`demo.cpp`](./demo.cpp) 中的注释，它实质上是本框架的详细文档，请务必仔细阅读。

#### 实现要求

- 你需要保证你的代码没有任何编译警告
- 只能修改 gemm.cpp 文件
- 不能使用除了 dtype_ptr 和 reg 外的任何类型
- 不能申请任何内存，如果你需要内存来暂存东西，而不是寄存器，你可以使用 buffer 字段
- 禁止利用任何二进制技巧在一个寄存器/内存上，存储多个数值
- 严格按照框架的假设来设计代码，不允许绕过框架或者改变框架，即不允许利用漏洞，注意，这意味着请再次仔细阅读 [`demo.cpp`](./demo.cpp)，彻底理解这个框架
- 你的代码需要通过我们提供的在线测试

## 评测与打分

<!-- TODO -->

## 后记

为了方便，我们的故事背景使用 CPU 来展开，但其实主流架构的 CPU 上并没有那么多寄存器，
我们的题目其实是受到了 GPU 的全局内存和共享内存的启发，除了网络上大量的 CacheLab 资料，
想要了解的同学还可以参考以下内容来帮助你们优化实现：

- [深入浅出GPU优化系列：GEMM优化（一）](https://zhuanlan.zhihu.com/p/435908830)