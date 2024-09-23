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
    test_case(case0_m,
              case0_n,
              case0_p,
              gemm_case0);
}

void case1() {
    test_case(case1_m,
              case1_n,
              case1_p,
              gemm_case1);
}

void case2() {
    test_case(case2_m,
              case2_n,
              case2_p,
              gemm_case2);
}

void case3() {
    test_case(case3_m,
              case3_n,
              case3_p,
              gemm_case3);
}

void case0_baseline() {
    test_case(case0_m,
              case0_n,
              case0_p,
              gemm_case0_baseline);
}

void case1_baseline() {
    test_case(case1_m,
              case1_n,
              case1_p,
              gemm_case1_baseline);
}

void case2_baseline() {
    test_case(case2_m,
              case2_n,
              case2_p,
              gemm_case2_baseline);
}

void case3_baseline() {
    test_case(case3_m,
              case3_n,
              case3_p,
              gemm_case3_baseline);
}