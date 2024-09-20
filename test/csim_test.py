import subprocess
import argparse
from utils import *


def output_results(results: list):
    results2 = results.copy()
    total_ok = sum(1 for status, *_ in results2 if status == "OK ")
    score = 100 * (total_ok / len(results2)) ** (1 / 4)
    results2.insert(
        0,
        [
            "status",
            "trace_file",
            "(s, E, b)",
            "ref: (hits, misses, evictions)",
            "handin: (hits, misses, evictions)",
        ],
    )
    print(format_table(results2))
    print(f"Score: {score:.2f}")
    return total_ok, score


def test_csim(ignore_ref=False):
    ref_executable = "csim-ref"
    ref_executable2 = "csim-ref2"
    handin_executable = "csim"
    trace_files = [
        "traces/yi2.trace",
        "traces/yi.trace",
        "traces/dave.trace",
        "traces/trans.trace",
        "traces/long.trace",
    ]
    results_file = ".csim_results"
    results = []
    subprocess.run(
        "make",
        check=True,
        shell=True,
        capture_output=True,
    )
    for s, E, b in [(5, 1, 5), (2, 4, 3), (4, 2, 4), (1, 1, 1)]:
        for trace_file in trace_files:
            if not ignore_ref:
                subprocess.run(
                    f"./{ref_executable} -s {s} -E {E} -b {b} -t {trace_file}",
                    check=True,
                    shell=True,
                    capture_output=True,
                )
                ref_results = parse_results_file(open(results_file, "r").read())
            else:
                ref_results = "Unknown"
            if False:  # debug only
                subprocess.run(
                    f"./{ref_executable2} -s {s} -E {E} -b {b} -t {trace_file}.old",
                    shell=True,
                    capture_output=True,
                )
                ref2_results = parse_results_file(open(results_file, "r").read())
                assert (
                    ref_results == ref2_results
                ), f"Unexpected results for {trace_file}"
            subprocess.run(
                f"./{handin_executable} -s {s} -E {E} -b {b} -t {trace_file}",
                check=True,
                shell=True,
                capture_output=True,
            )
            handin_results = parse_results_file(open(results_file, "r").read())
            status = "OK " if ref_results == handin_results else "ERROR"
            results.append((status, trace_file, (s, E, b), ref_results, handin_results))
    return output_results(results)
    # return results


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--no-linux",
        action="store_true",
    )
    args = parser.parse_args()
    test_csim(ignore_ref=args.no_linux)


if __name__ == "__main__":
    main()
