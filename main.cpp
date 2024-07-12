#include <iostream>
#include "common.h"
#include "gemm.h"
#include "matrix.h"

int main() {
    int m = 32;
    int n = 32;
    int p = 32;
    auto [A, B, C] = init(m, n, p);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << A[i * n + j] << " ";
        }
        std::cout << std::endl;
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            std::cout << B[i * p + j] << " ";
        }
        std::cout << std::endl;
    }
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            std::cout << C[i * p + j] << " ";
        }
        std::cout << std::endl;
    }
    // gemm(A, B, C, m, n, p);
    if (correct_check(A, B, C, m, n, p)) {
        std::cout << "correct" << std::endl;
    } else {
        std::cout << "wrong" << std::endl;
    }
    destroy();
}
