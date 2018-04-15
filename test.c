#include "cse320_functions.h"

int main(){
	int * test = cse320_malloc(4 * sizeof(int));
	test[0] = 4;
	test[1] = 8;
	test[3] = 11;
	printf("%d %d %d %d %p\n", test[0], test[1], test[3], addresses[size_alloc - 1].ref_count, addresses[size_alloc -1].addr);
	if (files[4].filename == NULL)
		printf("Jello\n");
	FILE* file = cse320_fopen("Makefile");
	cse320_clean();
	pid_t pid = cse320_fork();
	if (pid != 0)
		while(1);
	return 0;
}
