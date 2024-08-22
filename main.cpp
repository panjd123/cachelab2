#include "cachelab.h"

void case1() {
    auto [A, B, C, buffer] = init(32, 32, 32);
    gemm_case1(A, B, C, buffer);

    if (!correct_check(A, B, C, 32, 32, 32)) {
        throw std::runtime_error("Incorrect result");
    }

    print_log();
    destroy();
}
void case2() {
    auto [A, B, C, buffer] = init(64, 64, 64);
    gemm_case2(A, B, C, buffer);

    if (!correct_check(A, B, C, 64, 64, 64)) {
        throw std::runtime_error("Incorrect result");
    }

    print_log();
    destroy();
}
void case3() {
    auto [A, B, C, buffer] = init(67, 61, 67);
    gemm_case3(A, B, C, buffer);

    if (!correct_check(A, B, C, 67, 61, 67)) {
        throw std::runtime_error("Incorrect result");
    }

    print_log();
    destroy();
}
int main(int argc, char** argv) {
    if (argc != 2) {
        throw std::runtime_error("Usage: ./main case1/case2/case2");
    }
    if (std::string(argv[1]) == "case1") {
        case1();
    } else if (std::string(argv[1]) == "case2") {
        case2();
    } else if (std::string(argv[1]) == "case3") {
        case3();
    } else {
        throw std::runtime_error("Usage: ./main case1/case2/case2");
    }
}
