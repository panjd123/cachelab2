CC=g++
CFLAGS=-Wall -O0 -g -std=c++17 -fsanitize=address
CSIMFALGS=-Wall -O2 -g
# change to "CSIMFALGS=-Wall -O0 -g" for debugging

all: main demo csim

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

case0:
	./main case0 > case0.trace

case1:
	./main case1 > case1.trace

case2:
	./main case2 > case2.trace

case3:
	./main case3 > case3.trace

clean:
	rm -f main demo *.o csim