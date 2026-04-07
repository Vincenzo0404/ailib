
CC=g++

FLAGS=-O3 -Wall -std=c++20

# Targets to build
OBJS=main test_nqueens test_puzzle8 test_proteinfolding2d

all: $(OBJS)

main : main.cpp *.hpp policies/*.hpp problems/*.hpp datastructures/*.hpp
	$(CC) $(FLAGS) -o main main.cpp

test_nqueens: tests/test_nqueens.cpp *.hpp policies/*.hpp problems/*.hpp datastructures/*.hpp
	$(CC) $(FLAGS) -o test_nqueens tests/test_nqueens.cpp

test_puzzle8: tests/test_puzzle8.cpp *.hpp policies/*.hpp problems/*.hpp datastructures/*.hpp
	$(CC) $(FLAGS) -o test_puzzle8 tests/test_puzzle8.cpp

test_proteinfolding2d: tests/test_proteinfolding2d.cpp *.hpp policies/*.hpp problems/*.hpp datastructures/*.hpp
	$(CC) $(FLAGS) -o test_proteinfolding2d tests/test_proteinfolding2d.cpp

run: main
	./main

test_nq: test_nqueens
	./test_nqueens

test_p8: test_puzzle8
	./test_puzzle8

test_pf2d: test_proteinfolding2d
	./test_proteinfolding2d

clean:
	rm -rf main test_nqueens test_puzzle8 test_proteinfolding2d *.o