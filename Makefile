all:
	gcc -g part1.c -o part1
	gcc -c cse320_functions.c -o cse320_functions.o -lpthread -lrt
test:
	gcc test.c cse320_functions.o -o test

.PHONY: test
