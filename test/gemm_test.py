import subprocess
import os.path as osp
import argparse
from utils import *


def get_line_num(file) -> int:
    with open(file, "r") as f:
        return len(f.readlines())


def output_results(results: list, baseline: tuple):
    results2 = results.copy()
    weight = [0, 0.3, 0.3, 0.4]
    weighted_speedup = sum(
        speedup * w for (_, _, _, _, speedup), w in zip(results2, weight)
    )
    results2.insert(
        0,
        [
            "case",
            "miss_cache",
            "miss_reg",
            "latency",
            "speedup",
        ],
    )
    print(format_table(results2))
    print(f"Weighted Speedup: {weighted_speedup:.2f}")
    # return tuple([item for tup in results for item in tup[1:]] + [weighted_speedup])
    o_results = []
    for tup, b in zip(results, baseline):
        o_results.extend(tup[1:-1])
        o_results.append(b)
    o_results.append(weighted_speedup)
    return o_results


def test_gemm_case(case: str, no_linux=False) -> tuple:
    result = subprocess.run(
        f"make {case}" + (" NO_LINUX=1" if no_linux else ""),
        # check=True,
        shell=True,
        capture_output=True,
    )
    if result.returncode != 0:
        print(f"Failed on {case}")
        print(result.stderr.decode("utf-8"))
        raise Exception(f"Failed on {case}")
    miss_reg = get_line_num(f"gemm_traces/{case}.trace")
    miss_cache = parse_results_file(open(".csim_results", "r").read())[1]
    latency = 15 * miss_cache + miss_reg
    return case, miss_cache, miss_reg, latency


def test_gemm(ignore_submit=False, no_linux=False, baseline_only=False):
    if not ignore_submit:
        if not osp.exists(".access_key"):
            print("Please run bash submit_gemm.sh to setup the access key.")
            exit(0)

    # Local test
    results = []
    subprocess.run(["make", "-j"], check=True, shell=True, capture_output=True)

    baselines = []

    for case in [
        "case0_baseline",
        "case1_baseline",
        "case2_baseline",
        "case3_baseline",
    ]:
        result = test_gemm_case(case, no_linux=no_linux)
        baselines.append(result[-1])
        if baseline_only:
            results.append(list(result) + [1])

    if not baseline_only:
        for case, baseline in zip(["case0", "case1", "case2", "case3"], baselines):
            result = test_gemm_case(case, no_linux=no_linux)
            result = (*result, baseline / result[-1])
            results.append(result)

    o_results = output_results(results, baselines)

    if not ignore_submit:
        # Upload
        print("Leaderboard Submitting...")
        submission_result = subprocess.run(["bash", "submit_gemm.sh"], check=True)

    return o_results


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--no_linux", action="store_true")
    parser.add_argument("--baseline", action="store_true")
    args = parser.parse_args()
    test_gemm(no_linux=args.no_linux, baseline_only=args.baseline)


if __name__ == "__main__":
    main()
