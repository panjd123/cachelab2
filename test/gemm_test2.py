# This file is provided for the automation tool.
from gemm_test import test_gemm as _test_gemm


def test_csim2():
    result = _test_gemm()
    # miss_cache, miss_reg, latency, baseline

    latency_case1, baseline_case1 = result[2], result[3]
    assert latency_case1 < baseline_case1, "You should optimize case1"

    latency_case2, baseline_case2 = result[6], result[7]
    assert latency_case2 < baseline_case2, "You should optimize case2"

    latency_case3, baseline_case3 = result[10], result[11]
    assert latency_case3 < baseline_case3, "You should optimize case3"