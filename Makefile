CC=g++
CFLAGS=-Wall -O0

all: main

matrix.o: matrix.cpp
	$(CC) $(CFLAGS) -c matrix.cpp

gemm.o: gemm.cpp
	$(CC) $(CFLAGS) -c gemm.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

main: main.o gemm.o matrix.o
	$(CC) $(CFLAGS) -o main main.o gemm.o matrix.o

clean:
	rm -f main *.o