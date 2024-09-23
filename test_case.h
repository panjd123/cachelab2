#pragma once
#include "common.h"

#define case0_m 2
#define case0_n 2
#define case0_p 2

#define case1_m 16
#define case1_n 16
#define case1_p 16

#define case2_m 32
#define case2_n 32
#define case2_p 32

#define case3_m 31
#define case3_n 37
#define case3_p 31

void test_case(int m, int n, int p, void (*gemm_case)(dtype_ptr, dtype_ptr, dtype_ptr, dtype_ptr));
void case0();
void case1();
void case2();
void case3();
void case0_baseline();
void case1_baseline();
void case2_baseline();
void case3_baseline();