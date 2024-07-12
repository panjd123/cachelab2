#include "matrix.h"

namespace {
std::vector<int*> ptrs;
}

std::tuple<dtype_ptr, dtype_ptr, dtype_ptr, dtype_ptr> init(int m, int n, int p) {
    int* mem_pool = new int[m * n + n * p + m * p + BUFFER_SIZE];
    int* rawA = mem_pool;
    int* rawB = rawA + m * n;
    int* rawC = rawB + n * p;
    int* buffer = rawC + m * p;
    dtype_ptr::base = mem_pool;
    dtype_ptr::base_offset = 0x30000000;
    ptrs.push_back(mem_pool);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            rawA[i * n + j] = i * n + j;
        }
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            rawB[i * p + j] = i * p + j;
        }
    }
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            rawC[i * p + j] = 0;
        }
    }
    dtype_ptr A(rawA);
    dtype_ptr B(rawB);
    dtype_ptr C(rawC);
    dtype_ptr D(buffer);
    return std::make_tuple(A, B, C, D);
}

bool correct_check(dtype_ptr A, dtype_ptr B, dtype_ptr C, int m, int n, int p) {
    int* rawA = A.ptr_;
    int* rawB = B.ptr_;
    int* rawC = C.ptr_;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            int sum = 0;
            for (int k = 0; k < n; k++) {
                sum += rawA[i * n + k] * rawB[k * p + j];
            }
            if (sum != rawC[i * p + j]) {
                return false;
            }
        }
    }
    return true;
}

void destroy() {
    for (int* ptr : ptrs) {
        delete[] ptr;
    }
}