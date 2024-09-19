// student id: 2021201626
// please change the above line to your student id

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void printSummary(int hits, int misses, int evictions) {
  printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
  FILE *output_fp = fopen(".csim_results", "w");
  assert(output_fp);
  fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
  fclose(output_fp);
}

void printHelp() {
  printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n"
         "Options:\n"
         "  -h         Print this help message.\n"
         "  -v         Optional verbose flag.\n"
         "  -s <num>   Number of set index bits.\n"
         "  -E <num>   Number of lines per set.\n"
         "  -b <num>   Number of block offset bits.\n"
         "  -t <file>  Trace file.\n\n"
         "Examples:\n"
         "  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n"
         "  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

int main(int argc, char *argv[]) {
  printSummary(0, 0, 0);
  return 0;
}
