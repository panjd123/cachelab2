#pragma once

#include <tuple>
#include <vector>
#include "common.h"

namespace {
std::vector<int*> ptrs;
}

inline std::tuple<dtype_ptr, dtype_ptr, dtype_ptr> init(int m, int n, int p) {
    int* rawA = new int(m * n);
    int* rawB = new int(n * p);
    int* rawC = new int(m * p);
    ptrs.push_back(rawA);
    ptrs.push_back(rawB);
    ptrs.push_back(rawC);
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
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            std::cout << "#" << B[i * p + j] << " ";
        }
        std::cout << std::endl;
    }
    return std::make_tuple(A, B, C);
}

inline bool correct_check(dtype_ptr A, dtype_ptr B, dtype_ptr C, int m, int n, int p) {
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

inline void destroy() {
    for (int* ptr : ptrs) {
        delete[] ptr;
    }
}