all:
	gcc -g part1.c -o part1
	gcc -g -pthread -c cse320_functions.c -o cse320_functions.o
test:
	gcc -g test.c cse320_functions.o -o test -pthread

.PHONY: test
