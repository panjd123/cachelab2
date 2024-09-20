CC=g++
CFLAGS=-Wall -O0 -g -std=c++17 -fsanitize=address
CSIMFALGS=-Wall -O2 -g
# change to "CSIMFALGS=-Wall -O0 -g" for debugging

all: main demo csim # handin

matrix.o: matrix.cpp matrix.h common.h
	$(CC) $(CFLAGS) -c matrix.cpp

gemm.o: gemm.cpp matrix.h common.h
	$(CC) $(CFLAGS) -c gemm.cpp

test_case.o: test_case.cpp gemm.h matrix.h common.h
	$(CC) $(CFLAGS) -c test_case.cpp

main.o: main.cpp gemm.h matrix.h common.h test_case.h
	$(CC) $(CFLAGS) -c main.cpp

main: main.o gemm.o matrix.o test_case.o
	$(CC) $(CFLAGS) -o main main.o gemm.o matrix.o test_case.o

demo.o: demo.cpp gemm.h matrix.h common.h
	$(CC) $(CFLAGS) -c demo.cpp

demo: demo.o gemm.o matrix.o
	$(CC) $(CFLAGS) -o demo demo.o gemm.o matrix.o

csim: csim.c
	$(CC) $(CSIMFALGS) -o csim csim.c

csim-ref: csim-ref.c
	$(CC) -Wall -O3 -o csim-ref csim-ref.c

case-all: case0 case1 case2 case3

case0:
	mkdir -p gemm_traces
	./main case0 > gemm_traces/case0.trace
	./csim-ref -s 5 -E 1 -b 5 -t gemm_traces/case0.trace

case1:
	mkdir -p gemm_traces
	./main case1 > gemm_traces/case1.trace
	./csim-ref -s 5 -E 1 -b 5 -t gemm_traces/case1.trace

case2:
	mkdir -p gemm_traces
	./main case2 > gemm_traces/case2.trace
	./csim-ref -s 5 -E 1 -b 5 -t gemm_traces/case2.trace

case3:
	mkdir -p gemm_traces
	./main case3 > gemm_traces/case3.trace
	./csim-ref -s 5 -E 1 -b 5 -t gemm_traces/case3.trace

clean:
	rm -f main demo *.o csim handin.tar

handin:
	tar cvf handin.tar csim.c gemm.cpp report/report.pdf