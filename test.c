#include "cse320_functions.h"

int main(){
	cse320_init();
	int i;
	for (i = 0; i < 25; i++){
		cse320_malloc(sizeof(int) * 4);
	}
	cse320_fopen("part2.c", "r");
	cse320_fclose("part2.c");
	cse320_fopen("part1.c", "r+");
	cse320_clean();
	return 0;
}
