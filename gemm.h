#pragma once
#include "common.h"
#include "matrix.h"

void gemm(dtype_ptr A, dtype_ptr B, dtype_ptr C, int m, int n, int p);