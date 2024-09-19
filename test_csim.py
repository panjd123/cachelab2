import subprocess
import argparse


def format_table(data):
    column_widths = [max(len(str(item)) for item in col) for col in zip(*data)]
    table_str = ""
    for i, row in enumerate(data):
        table_str += (
            " | ".join(
                f"{str(item).ljust(width)}" for item, width in zip(row, column_widths)
            )
            + "\n"
        )
        if i == 0:
            table_str += "-+-".join("-" * width for width in column_widths) + "\n"

    return table_str


def output_results(results: list):
    total_ok = sum(1 for status, *_ in results if status == "OK ")
    score = 100 * (total_ok / len(results)) ** (1 / 4)
    results.insert(
        0,
        [
            "status",
            "trace_file",
            "(s, E, b)",
            "ref: (hits, misses, evictions)",
            "handin: (hits, misses, evictions)",
        ],
    )
    print(format_table(results))
    print(f"Score: {score:.2f}")


def parse_csim_output(text) -> tuple:
    "hits:15 misses:1 evictions:0"
    return tuple(
        map(
            lambda x: int(x.split(":")[1]),
            text.strip().split(" "),
        )
    )


def parse_results_file(text) -> tuple:
    "15 1 0"
    return tuple(map(int, text.strip().split(" ")))


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
    for s, E, b in [(5, 1, 5), (2, 4, 3), (4, 2, 4), (1, 1, 1)]:
        for trace_file in trace_files:
            if not ignore_ref:
                subprocess.run(
                    f"./{ref_executable} -s {s} -E {E} -b {b} -t {trace_file}",
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
                shell=True,
                capture_output=True,
            )
            handin_results = parse_results_file(open(results_file, "r").read())
            status = "OK " if ref_results == handin_results else "ERROR"
            results.append((status, trace_file, (s, E, b), ref_results, handin_results))
    output_results(results)


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
