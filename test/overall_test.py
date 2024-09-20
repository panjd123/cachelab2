from csim_test import test_csim
from gemm_test import test_gemm


def main():
    overall_results_file = ".overall_results"
    results_csim = test_csim()
    results_gemm = test_gemm(ignore_submit=True)

    with open(overall_results_file, "w") as f:
        f.write(" ".join(map(str, results_csim)) + "\n")
        f.write(" ".join(map(str, results_gemm)) + "\n")

    print(results_csim)
    print(results_gemm)


if __name__ == "__main__":
    main()
