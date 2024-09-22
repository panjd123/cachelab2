#include "gemm.h"

#define m 2
#define n 2
#define p 2

void gemm_case0_baseline(dtype_ptr A, dtype_ptr B, dtype_ptr C, dtype_ptr buffer) {  // allocate 0 1 2 3
    for (reg i = 0; i < m; ++i) {                                                    // allocate 4
        for (reg j = 0; j < p; ++j) {                                                // allocate 5
            reg tmpc = 0;                                                            // allocate 6
            for (reg k = 0; k < n; ++k) {                                            // allocate 7
                reg tmpa = A[i * n + k];                                             // allocate 8
                reg tmpb = B[k * p + j];                                             // allocate 9
                tmpc += tmpa * tmpb;
            }  // free 9 8
            // free 7
            C[i * p + j] = tmpc;
        }  // free 6
        // free 5
    }
    // free 4
}  // free 3 2 1 0

#undef m
#undef n
#undef p

#define m 8
#define n 8
#define p 8

void gemm_case1_baseline(dtype_ptr A, dtype_ptr B, dtype_ptr C, dtype_ptr buffer) {
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

#undef m
#undef n
#undef p

#define m 16
#define n 16
#define p 16

void gemm_case2_baseline(dtype_ptr A, dtype_ptr B, dtype_ptr C, dtype_ptr buffer) {
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

#undef m
#undef n
#undef p

#define m 17
#define n 13
#define p 17

void gemm_case3_baseline(dtype_ptr A, dtype_ptr B, dtype_ptr C, dtype_ptr buffer) {
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

#undef m
#undef n
#undef p