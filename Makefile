CC=g++
CFLAGS=-Wall -O0 -g -std=c++17 -fsanitize=address
CSIMFALGS=-Wall -O2 -g
# change to "CSIMFALGS=-Wall -O0 -g" for debugging

case_s=4
case_E=1
case_b=4

all: main demo csim # handin

matrix.o: matrix.cpp matrix.h common.h
	$(CC) $(CFLAGS) -c matrix.cpp

gemm.o: gemm.cpp matrix.h common.h
	$(CC) $(CFLAGS) -c gemm.cpp

gemm_baseline.o: gemm_baseline.cpp matrix.h common.h
	$(CC) $(CFLAGS) -c gemm_baseline.cpp

test_case.o: test_case.cpp gemm.h matrix.h common.h
	$(CC) $(CFLAGS) -c test_case.cpp

main.o: main.cpp gemm.h matrix.h common.h test_case.h
	$(CC) $(CFLAGS) -c main.cpp

main: main.o gemm.o matrix.o test_case.o gemm_baseline.o
	$(CC) $(CFLAGS) -o main main.o gemm.o gemm_baseline.o matrix.o test_case.o

demo.o: demo.cpp gemm.h matrix.h common.h
	$(CC) $(CFLAGS) -c demo.cpp

demo: demo.o gemm.o matrix.o
	$(CC) $(CFLAGS) -o demo demo.o gemm.o matrix.o

csim: csim.c
	$(CC) $(CSIMFALGS) -o csim csim.c

csim-ref: csim-ref.c
	$(CC) -Wall -O3 -o csim-ref csim-ref.c

case-all: case0 case1 case2 case3

case%:
	mkdir -p gemm_traces
	./main $@ > gemm_traces/$@.trace
	@if [ "$(NO_LINUX)" = "true" ]; then \
		./csim -s $(case_s) -E $(case_E) -b $(case_b) -t gemm_traces/$@.trace; \
	else \
		./csim-ref -s $(case_s) -E $(case_E) -b $(case_b) -t gemm_traces/$@.trace; \
	fi

clean:
	rm -f main demo *.o csim handin.tar

handin:
	tar cvf handin.tar csim.c gemm.cpp report/report.pdf