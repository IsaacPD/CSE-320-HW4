#include <stdio.h>
#include <stdlib.h>

struct addr_in_use{
	void* addr;
	int ref_count;
};
struct files_in_use{
	char* filename;
	int ref_count;
};

struct addr_in_use 	addresses[25];
struct files_in_use files[25];

void * 	cse320_malloc(size_t);
void 	cse320_free(void *);
FILE * 	cse320_fopen(const char *, const char *);
int 	cse320_fclose(const char *);
void	cse320_clean();
pid_t	cse320_fork();
int		cse320_settimer(int);
