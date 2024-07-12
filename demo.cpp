#include "cachelab.h"

// 这个函数会演示部分你可以/不可以使用的语法，事实上，只要不在编译期报错的语法就是可以写的
// 注意，你不能自己申请额外的内存，即不能使用 new/malloc，全局变量只要是 reg 就是被允许的
void demo(dtype_ptr A, dtype_ptr B, dtype_ptr C, dtype_ptr buffer) {
    reg a = A[0];                          // 初始化一个寄存器，并读内存到寄存器
    reg b = 100;                           // 初始化一个寄存器，并初始化一个常数，这不会导致内存访问，默认初始化为 0
    b = B[10];                             // 读内存到寄存器
    B[10] = a;                             // 将寄存器写入内存
    dtype_ptr subC = C + 10;               // 初始化一个指针
    reg c = *subC;                         // 用 * 操作符读内存到寄存器
    reg d = A[a * b * (c + 5)];            // 寄存器运算的结果可以作为下标使用
    dtype_ptr subA = A + a * (b + 5) * c;  // 也可以用于指针运算

    // 我们不允许使用的语法
    // A[0] * B[0];      // 内存上的数据不能直接参与计算
    // A[0] * a;         // 内存上的数据不能直接参与计算
    // A[0] = *(B + 1);  // 不能直接内存到内存赋值

    // 其他常见的指针运算在 dtype_ptr 上一般也是支持的

    // 这里有一个实现上的漏洞，
    // 对于二元计算，即两个操作数，一个结果的情况，我们是不需要临时寄存器的
    // 然而你可以写出非二元计算的复杂式子
    reg total;
    total = (b - a) * (c - a);  // no need for a temporary register
    // 我们不推荐你利用这个漏洞，你可以写成这样：
    reg tmp = b - a;  // actually, you need a temporary register
    reg total = c - a;
    total = tmp * total;
    // 但是，因为在 cachelab 中，助教没有想到可以利用这个漏洞的场景（没有复杂计算），所以这个漏洞应该不会导致问题
}

int main() {
    auto [A, B, C, buffer] = init(32, 32, 32);
    demo(A, B, C, buffer);
}