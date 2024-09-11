#pragma once

#include <tuple>
#include <vector>
#include "common.h"

#define BUFFER_SIZE 64
namespace {
extern std::vector<int*> ptrs;
}

std::tuple<dtype_ptr, dtype_ptr, dtype_ptr, dtype_ptr> init(int m, int n, int p);

bool correct_check(dtype_ptr A, dtype_ptr B, dtype_ptr C, int m, int n, int p);

void destroy();