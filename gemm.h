#pragma once
#include "common.h"
#include "matrix.h"

void gemm_case1(dtype_ptr A, dtype_ptr B, dtype_ptr C, dtype_ptr buffer);
void gemm_case2(dtype_ptr A, dtype_ptr B, dtype_ptr C, dtype_ptr buffer);
void gemm_case3(dtype_ptr A, dtype_ptr B, dtype_ptr C, dtype_ptr buffer);