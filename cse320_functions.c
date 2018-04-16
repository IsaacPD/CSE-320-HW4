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

int timer = 5;
int alarmset = 0;

struct addr_in_use * addresses;
struct addr_in_use * addTail;
struct files_in_use * files;
struct files_in_use * fileTail;

sem_t sem_file;
sem_t sem_alloc;

void cse320_init(){
	sem_init(&sem_file, 0, 1);
	sem_init(&sem_alloc, 0, 1);
	addresses = malloc(sizeof(struct addr_in_use));
	addresses->ref_count = -1;
	files = malloc(sizeof(struct files_in_use));
	files->ref_count = -1;

	addTail = addresses;
	fileTail = files;
}

void * cse320_malloc(size_t size){
	sigset_t block, prev;
	sigfillset(&block);
	void * ret = malloc(size);
	sigprocmask(SIG_BLOCK, &block, &prev);

	//LOCK HERE
	sem_wait(&sem_alloc);
	addTail->next = malloc(sizeof(struct addr_in_use));
	addTail = addTail->next;

	addTail->addr = ret;
	addTail->ref_count = 1;	
	
	//UNLOCK HERE
	sem_post(&sem_alloc);
	
	sigprocmask(SIG_SETMASK, &prev, NULL);
	return ret;
}

void cse320_free(void * ptr){
	sigset_t block, prev;
	sigfillset(&block);	
	int i;
	//LOCK
	sigprocmask(SIG_BLOCK, &block, &prev);
	sem_wait(&sem_alloc);
	struct addr_in_use * cursor = addresses;
	while(cursor->next){
		if (cursor->next->addr == ptr){
			if (cursor->next->ref_count == 0){
				//UNLOCK
				sem_post(&sem_alloc);
				sigprocmask(SIG_SETMASK, &prev, NULL);
				printf("Free: Double free attempt\n");
				errno = 13;
				exit(-1);
			}
			free(ptr);
			struct addr_in_use * toFree = cursor->next;	
			if (toFree == addTail)
				addTail = cursor;
			cursor->next = toFree->next;
			free(toFree);
			//UNLOCK
			sem_post(&sem_alloc);
			sigprocmask(SIG_SETMASK, &prev, NULL);
			return;
		}
		cursor = cursor->next;
	}
	//UNLOCK
	sem_post(&sem_alloc);
	sigprocmask(SIG_SETMASK, &prev, NULL);
	printf("Free: Illegal address\n");
	errno = EFAULT;
	exit(-1);
}

FILE * cse320_fopen(const char * filename, const char * mode){
	sigset_t block, prev;
	sigfillset(&block);
	
	int i;
	//LOCK
	sigprocmask(SIG_BLOCK, &block, &prev);
	sem_wait(&sem_file);
	struct files_in_use * cursor = files;
	while(cursor->next) {
		if (strcmp(cursor->next->filename, filename) == 0){
			cursor->next->ref_count++;
			//UNLOCK
			sigprocmask(SIG_SETMASK, &prev, NULL);
			sem_post(&sem_file);
			return cursor->next->file;
		}
		cursor = cursor->next;
	}
	FILE * file = fopen(filename, mode);
	if (file == NULL) return NULL;
	fileTail->next = malloc(sizeof(struct files_in_use));
	fileTail = fileTail->next;
	fileTail->file = file;
	fileTail->filename = filename;
	fileTail->ref_count = 1;
	//UNLOCK
	sem_post(&sem_file);
	sigprocmask(SIG_SETMASK, &prev, NULL);
	return file;
}

void cse320_fclose(const char* filename){
	sigset_t block, prev;
	sigfillset(&block);
	int i;
	//LOCK
	sigprocmask(SIG_BLOCK, &block, &prev);
	sem_wait(&sem_file);
	struct files_in_use * cursor = files;
	while(cursor->next) {
		if (strcmp(cursor->next->filename, filename) == 0){
			if(cursor->next->ref_count == 0){
				//UNLOCK
				sem_post(&sem_file);
				sigprocmask(SIG_SETMASK, &prev, NULL);
				printf("Close: Ref count is zero\n");
				errno = EINVAL;
				exit(-1);
			}
			cursor->next->ref_count--;
			if (cursor->next->ref_count == 0){
				fclose(cursor->next->file);
				struct files_in_use * toFree = cursor->next;
				if (toFree == fileTail)
					fileTail = cursor;
				cursor->next = toFree->next;
				free(toFree);
			}
			//UNLOCK
			sem_post(&sem_file);
			sigprocmask(SIG_SETMASK, &prev, NULL);
			return;
		}
		cursor = cursor->next;
	}
	//UNLOCK
	sem_post(&sem_file);
	sigprocmask(SIG_SETMASK, &prev, NULL);
	printf("Close: Illegal filename\n");
	errno = ENOENT;
	exit(-1);
}

void cse320_clean(){
	sigset_t block, prev;
	sigfillset(&block);
	int i;
	//LOCK
	sigprocmask(SIG_BLOCK, &block, &prev);
	sem_wait(&sem_alloc);
	sem_wait(&sem_file);
	struct addr_in_use * cursor = addresses;
	while (cursor->next){
		if(cursor->next->ref_count > 0){
			free(cursor->next->addr);
		}
		struct addr_in_use * next = cursor->next;
		free(cursor);
		cursor = next;
	}
	free(cursor);
	struct files_in_use * curs = files;
	while (curs->next){
		if (curs->next->ref_count > 0){
			fclose(curs->next->file);
		}
		struct files_in_use * next = curs->next;
		free(curs);
		curs = next;
	}
	free(curs);

	//UNLOCK
	sem_post(&sem_alloc);
	sem_post(&sem_file);
	sem_destroy(&sem_alloc);
	sem_destroy(&sem_file);
	sigprocmask(SIG_SETMASK, &prev, NULL);
}

void handler(int sig){
	sigset_t block, prev;
	sigfillset(&block);
	sigprocmask(SIG_BLOCK, &block, &prev);
	while (waitpid(-1, NULL, WNOHANG) > 0){;}
	alarm(timer);
	sigprocmask(SIG_SETMASK, &prev, NULL);
	return;
}

pid_t cse320_fork(){
	if (alarmset == 0){
		signal(SIGALRM, handler);
	}
	pid_t pid;
	if ((pid = fork()) > 0){
		if (alarmset == 0){
			alarmset = 1;
			alarm(timer);
		}
	}
	return pid;
}

void cse320_settimer(int time){
	timer = time;
}
