import subprocess
from test_csim import parse_results_file, format_table


def get_line_num(file) -> int:
    with open(file, "r") as f:
        return len(f.readlines())


def output_results(results: list):
    results.insert(
        0,
        [
            "case",
            "miss_cache",
            "miss_reg",
            "latency",
            "score",
        ],
    )
    print(format_table(results))


def get_latency(case: str = "case1") -> int:
    result = subprocess.run(f"make {case}", shell=True, capture_output=True)
    miss_reg = get_line_num(f"gemm_traces/{case}.trace")
    miss_cache = parse_results_file(open(".csim_results", "r").read())[1]
    latency = 15 * miss_cache + miss_reg
    score = "Unknown"
    return case, miss_cache, miss_reg, latency, score


def main():
    # Local test
    results = []
    subprocess.run(["make"], shell=True, capture_output=True)

    for case in ["case1", "case2", "case3"]:
        result = get_latency(case)
        results.append(result)

    output_results(results)

    # Upload
    result = subprocess.run(
        ["bash", "submit_gemm.sh"], check=True, capture_output=True, text=True
    )
    print("排行榜提交结果：")
    print(result.stdout.strip())


if __name__ == "__main__":
    main()
