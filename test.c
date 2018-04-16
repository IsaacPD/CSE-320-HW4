#include "cse320_functions.h"

int main(){
	cse320_init();
	int i;
	for (i = 0; i < 26; i++){
		cse320_malloc(sizeof(int) * 4);
	}
	cse320_clean();
	return 0;
}
