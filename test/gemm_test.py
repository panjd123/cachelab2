import subprocess
import os.path as osp
import argparse
from utils import *


def get_line_num(file) -> int:
    with open(file, "r") as f:
        return len(f.readlines())


def output_results(results: list, baseline: tuple):
    results2 = results.copy()
    weight = [0.3, 0.3, 0.4]
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


def test_gemm_case(case: str, baseline: int, no_linux=False) -> int:
    result = subprocess.run(
        f"make {case}" if not no_linux else f"make {case}_no_linux",
        check=True,
        shell=True,
        capture_output=True,
    )
    miss_reg = get_line_num(f"gemm_traces/{case}.trace")
    miss_cache = parse_results_file(open(".csim_results", "r").read())[1]
    latency = 15 * miss_cache + miss_reg
    speedup = baseline / latency
    return case, miss_cache, miss_reg, latency, speedup


def test_gemm(ignore_submit=False, no_linux=False):
    if not ignore_submit:
        if not osp.exists(".access_key"):
            print("Please run bash submit_gemm.sh to setup the access key.")
            exit(0)

    # Local test
    results = []
    subprocess.run(["make"], check=True, shell=True, capture_output=True)

    for case, baseline in [
        ("case0", 35),
        ("case1", 128450),
        ("case2", 4704384),
        ("case3", 4610517),
    ]:
        result = test_gemm_case(case, baseline, no_linux=no_linux)
        results.append(result)

    o_results = output_results(results, (128450, 4704384, 4610517))

    if not ignore_submit:
        # Upload
        submission_result = subprocess.run(
            ["bash", "submit_gemm.sh"], check=True, capture_output=True, text=True
        )
        print("Leaderboard Submission Feedback:")
        print(submission_result.stdout.strip())

    return o_results


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--no-linux", action="store_true")
    args = parser.parse_args()
    test_gemm(no_linux=args.no_linux)


if __name__ == "__main__":
    main()
