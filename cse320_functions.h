#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#ifndef CSE320_FUNCTIONS_H
#define CSE320_FUNCTIONS_H
 

struct addr_in_use{
	void* addr;
	int ref_count;
};
struct files_in_use{
	const char* filename;
	int ref_count;
	FILE * file;
};

/*
 * Takes in a single parameter, the size in bytes to be allocated,
 * Returns a pointer to the beginning of this place in memory.
 */
void * 	cse320_malloc(size_t);

/*
 * 
 */
int 	cse320_free(void *);
FILE * 	cse320_fopen(const char *);
int 	cse320_fclose(const char *);
void	cse320_clean();
pid_t	cse320_fork();
int		cse320_settimer(int);

#endif
