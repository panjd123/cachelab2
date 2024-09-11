#pragma once
#include "common.h"

void test_case(int m, int n, int p, void (*gemm_case)(dtype_ptr, dtype_ptr, dtype_ptr, dtype_ptr));
void case0();
void case1();
void case2();
void case3();