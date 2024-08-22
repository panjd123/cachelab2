CC=g++
CFLAGS=-Wall -O0 -g -std=c++17 -fsanitize=address

all: main demo

matrix.o: matrix.cpp matrix.h common.h
	$(CC) $(CFLAGS) -c matrix.cpp

gemm.o: gemm.cpp matrix.h common.h
	$(CC) $(CFLAGS) -c gemm.cpp

main.o: main.cpp gemm.h matrix.h common.h
	$(CC) $(CFLAGS) -c main.cpp

main: main.o gemm.o matrix.o
	$(CC) $(CFLAGS) -o main main.o gemm.o matrix.o

demo.o: demo.cpp gemm.h matrix.h common.h
	$(CC) $(CFLAGS) -c demo.cpp

demo: demo.o gemm.o matrix.o
	$(CC) $(CFLAGS) -o demo demo.o gemm.o matrix.o

test:
	./demo

case1:
	./main case1 > case1.trace

case2:
	./main case2 > case2.trace

case3:
	./main case3 > case3.trace

clean:
	rm -f main demo *.o