#include "cachelab.h"

void test_case(int m, int n, int p, void (*gemm_case)(dtype_ptr, dtype_ptr, dtype_ptr, dtype_ptr)) {
    int *rawA, *rawB, *rawC;
    {
        auto [A, B, C, buffer] = init(m, n, p);
        rawA = A.ptr_;
        rawB = B.ptr_;
        rawC = C.ptr_;
        gemm_case(std::move(A), std::move(B), std::move(C), std::move(buffer));
    }

    if (!correct_check(rawA, rawB, rawC, m, n, p)) {
        throw std::runtime_error("Incorrect result");
    }

    print_log();
    destroy();
}

void case0() {
    test_case(32, 32, 32, gemm_case0);
}

void case1() {
    test_case(32, 32, 32, gemm_case1);
}

void case2() {
    test_case(64, 64, 64, gemm_case2);
}

void case3() {
    test_case(67, 61, 67, gemm_case3);
}