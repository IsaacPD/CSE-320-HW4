#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include <string.h>
#include <wait.h>
#include <sys/types.h>
#include <signal.h>
#include "cse320_functions.h"

int size_alloc = 0;
int files_opened = 0;
int timer = 5;

struct addr_in_use addresses[25];
struct files_in_use files[25];

//TODO add semaphore for thread safety
void * cse320_malloc(size_t size){
	sigset_t block, prev;
	sigfillset(&block);
	if (size_alloc >= 25){
		printf("Not enough memory\n");
		errno = ENOMEM;
		return (void*)-1;
	}
	void * ret = malloc(size);
	sigprocmask(SIG_BLOCK, &block, &prev);

	//LOCJ HERE
	addresses[size_alloc].addr = ret;
	addresses[size_alloc].ref_count = 1;	
	size_alloc++;
	//UNLOCK HERE

	sigprocmask(SIG_SETMASK, &prev, NULL);
	return ret;
}

int cse320_free(void * ptr){	
	int i;
	for (i = 0; i < size_alloc; i++){
		if (addresses[i].addr == ptr){
			if (addresses[i].ref_count == 0){
				printf("Free: Double free attempt\n");
				errno = 13;
				return -1;
			}
			free(ptr);
			addresses[i].ref_count = 0;
			return 0;
		}
	}
	printf("Free: Illegal address\n");
	errno = EFAULT;
	return -1;
}

FILE * cse320_fopen(const char * filename){
	int i;
	for (i = 0; i < files_opened; i++){
		if (strcmp(files[i].filename, filename) == 0){
			files[i].ref_count++;
			return files[i].file;
		}
	}
	files[files_opened].file = fopen(filename, "rw+");
	files[files_opened].filename = filename;
	files[files_opened].ref_count = 1;
	files_opened++;

	return files[files_opened-1].file;
}

int cse320_fclose(const char* filename){
	int i;
	for (i = 0; i <files_opened; i++){
		if (strcmp(files[i].filename, filename) == 0){
			if(files[i].ref_count == 0){
				printf("Close: Ref count is zero\n");
				errno = EINVAL;
				return -1;
			}
			files[i].ref_count--;
			return 0;
		}
	}
	printf("Close: Illegal filename\n");
	errno = ENOENT;
	return -1;
}

void cse320_clean(){
	int i;
	//LOCK
	for (i = 0; i < size_alloc; i++){
		if(addresses[i].ref_count > 0){
			free(addresses[i].addr);
		}
	}
	for (i = 0; i < files_opened; i++){
		fclose(files[i].file);
	}
	//UNLOCK
}

void handler(int sig){
	sigset_t block, prev;
	sigfillset(&block);
	sigprocmask(SIG_BLOCK, &block, &prev);
	while (wait(NULL) > 0){;}
	sigprocmask(SIG_SETMASK, &prev, NULL);
	return;
}

pid_t cse320_fork(){
	signal(SIGALRM, handler);
	pid_t pid;
	if ((pid = fork()) > 0){
		alarm(timer);
	}
	return pid;
}

int cse320_settimer(int time){
	timer = time;
}
