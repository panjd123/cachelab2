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
    test_case(2, 2, 2, gemm_case0);
}

void case1() {
    test_case(8, 8, 8, gemm_case1);
}

void case2() {
    test_case(16, 16, 16, gemm_case2);
}

void case3() {
    test_case(17, 13, 17, gemm_case3);
}

void case0_baseline() {
    test_case(2, 2, 2, gemm_case0_baseline);
}

void case1_baseline() {
    test_case(8, 8, 8, gemm_case1_baseline);
}

void case2_baseline() {
    test_case(16, 16, 16, gemm_case2_baseline);
}

void case3_baseline() {
    test_case(17, 13, 17, gemm_case3_baseline);
}