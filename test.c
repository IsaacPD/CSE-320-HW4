#include "cse320_functions.h"
#include <string.h>
#include <unistd.h>

int main(){
	cse320_init();
	int i;
	for (i = 0; i < 25; i++){
		cse320_malloc(sizeof(int) * 4);
	}
	char s[10];
	strcpy(s, "part1.c");
	cse320_fopen(s, "r");
	s[0] = '\0';
	strcpy(s, "README.md");
	cse320_fopen(s, "r+");
	cse320_clean();
	return 0;
}
