#include "gemm.h"

void gemm(dtype_ptr A, dtype_ptr B, dtype_ptr C, int m, int n, int p) {
    reg tmpc;  // sum
    reg tmpa;
    reg tmpb;
    std::cout << "????" << std::endl;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            tmpc = 0;
            for (int k = 0; k < n; k++) {
                tmpa = A[i * n + k];
                tmpb = B[k * p + j];
                tmpc += tmpa * tmpb;
                std::cout << "tmpa: " << tmpa << ", tmpb: " << tmpb << ", tmpc: " << tmpc << std::endl;
            }
            C[i * p + j] = tmpc;
        }
    }
}