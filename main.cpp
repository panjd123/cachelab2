#include "cachelab.h"

int main() {
    auto [A, B, C, buffer] = init(32, 32, 32);
    gemm_case1(A, B, C, buffer);

    if (!correct_check(A, B, C, 32, 32, 32)) {
        throw std::runtime_error("Incorrect result");
    }

    print_log();
    destroy();
}
