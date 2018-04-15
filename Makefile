all:
	gcc -g part1.c -o part1
	gcc -pthread -c cse320_functions.c -o cse320_functions.o
test:
	gcc test.c cse320_functions.o -o test -pthread

.PHONY: test
