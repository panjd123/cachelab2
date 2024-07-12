CC=g++
CFLAGS=-Wall -O0 -g -std=c++17 -fsanitize=address

all: main

matrix.o: matrix.cpp matrix.h common.h
	$(CC) $(CFLAGS) -c matrix.cpp

gemm.o: gemm.cpp matrix.h common.h
	$(CC) $(CFLAGS) -c gemm.cpp

main.o: main.cpp gemm.h matrix.h common.h
	$(CC) $(CFLAGS) -c main.cpp

main: main.o gemm.o matrix.o
	$(CC) $(CFLAGS) -o main main.o gemm.o matrix.o

clean:
	rm -f main *.o