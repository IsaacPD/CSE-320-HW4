#include "cse320_functions.h"

int main(){
	cse320_init();
	int * test = cse320_malloc(4 * sizeof(int));
	test[0] = 4;
	test[1] = 8;
	test[3] = 11;
	cse320_free(test);
	
	FILE* file = cse320_fopen("Makefile", "r+");
	cse320_fork();
	cse320_fclose("Makefile");
	cse320_fopen("Makefile", "r+");
	cse320_clean();
	return 0;
}
