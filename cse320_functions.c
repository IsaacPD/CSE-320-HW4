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
int alarmset = 0;

struct addr_in_use addresses[25];
struct files_in_use files[25];

sem_t sem_file;
sem_t sem_alloc;

void cse320_init(){
	sem_init(&sem_file, 0, 1);
	sem_init(&sem_alloc, 0, 1);
}

void * cse320_malloc(size_t size){
	sigset_t block, prev;
	sigfillset(&block);
	if (size_alloc >= 25){
		fputs("Not enough memory\n", stdout);
		errno = ENOMEM;
		cse320_clean();
		exit(-1);
	}
	void * ret = malloc(size);
	
	sem_wait(&sem_alloc);
	sigprocmask(SIG_BLOCK, &block, &prev);
	//LOCK HERE
	addresses[size_alloc].addr = ret;
	addresses[size_alloc].ref_count = 1;	
	size_alloc++;
	//UNLOCK HERE
	sigprocmask(SIG_SETMASK, &prev, NULL);
	sem_post(&sem_alloc);

	return ret;
}

void cse320_free(void * ptr){
	sigset_t block, prev;
	sigfillset(&block);	
	int i;
	//LOCK
	sem_wait(&sem_alloc);
	sigprocmask(SIG_BLOCK, &block, &prev);
	for (i = 0; i < size_alloc; i++){
		if (addresses[i].addr == ptr){
			if (addresses[i].ref_count == 0){
				//UNLOCK
				sigprocmask(SIG_SETMASK, &prev, NULL);
				sem_post(&sem_alloc);
				fputs("Free: Double free attempt\n", stdout);
				errno = EADDRNOTAVAIL;
				cse320_clean();
				exit(-1);
			}
			free(ptr);
			addresses[i].addr = NULL;
			addresses[i].ref_count = 0;
			//UNLOCK
			sigprocmask(SIG_SETMASK, &prev, NULL);
			sem_post(&sem_alloc);
			return;
		}
	}
	//UNLOCK
	sem_post(&sem_alloc);
	sigprocmask(SIG_SETMASK, &prev, NULL);
	fputs("Free: Illegal address\n", stdout);
	errno = EFAULT;
	cse320_clean();
	exit(-1);
}

FILE * cse320_fopen(const char * filename, const char * mode){
	sigset_t block, prev;
	sigfillset(&block);
	if (files_opened >= 25){
		fputs("Too many opened files\n", stdout);
		errno = EMFILE;
		cse320_clean();
		exit(-1);
	}
	int i;
	//LOCK
	sem_wait(&sem_file);
	sigprocmask(SIG_BLOCK, &block, &prev);
	for (i = 0; i < files_opened; i++){
		if (strcmp(files[i].filename, filename) == 0){
			files[i].ref_count++;
			//UNLOCK
			sigprocmask(SIG_SETMASK, &prev, NULL);
			sem_post(&sem_file);
			return files[i].file;
		}
	}
	FILE * file = fopen(filename, mode);
	if (file == NULL){
		sigprocmask(SIG_SETMASK, &prev, NULL);
		sem_post(&sem_file);
	   	return NULL;
	}
	char * name = malloc(strlen(filename) + 1);
	strcpy(name, filename);
	files[files_opened].file = file;
	files[files_opened].filename = name;
	files[files_opened].ref_count = 1;
	files_opened++;
	//UNLOCK
	sigprocmask(SIG_SETMASK, &prev, NULL);
	sem_post(&sem_file);
	return files[files_opened-1].file;
}

void cse320_fclose(const char* filename){
	sigset_t block, prev;
	sigfillset(&block);
	int i;
	//LOCK
	sem_wait(&sem_file);
	sigprocmask(SIG_BLOCK, &block, &prev);
	for (i = 0; i <files_opened; i++){
		if (strcmp(files[i].filename, filename) == 0){
			if(files[i].ref_count == 0){
				//UNLOCK
				sigprocmask(SIG_SETMASK, &prev, NULL);
				sem_post(&sem_file);
				fputs("Close: Ref count is zero\n", stdout);
				errno = EINVAL;
				cse320_clean();
				exit(-1);
			}
			files[i].ref_count--;
			if (files[i].ref_count == 0){
				fclose(files[i].file);
				free(files[i].filename);
				files[i].filename = NULL;
				files[i].file = NULL;
			}
			//UNLOCK
			sigprocmask(SIG_SETMASK, &prev, NULL);
			sem_post(&sem_file);
			return;
		}
	}
	//UNLOCK
	sigprocmask(SIG_SETMASK, &prev, NULL);
	sem_post(&sem_file);
	fputs("Close: Illegal filename\n", stdout);
	errno = ENOENT;
	cse320_clean();
	exit(-1);
}

void cse320_clean(){
	sigset_t block, prev;
	sigfillset(&block);
	int i;
	sem_wait(&sem_alloc);
	sem_wait(&sem_file);
	sigprocmask(SIG_BLOCK, &block, &prev);
	for (i = 0; i < size_alloc; i++){
		if(addresses[i].ref_count > 0){
			free(addresses[i].addr);
		}
	}
	for (i = 0; i < files_opened; i++){
		if (files[i].ref_count > 0){
			fclose(files[i].file);
			free(files[i].filename);

		}
	}
	
	sigprocmask(SIG_SETMASK, &prev, NULL);
	sem_post(&sem_alloc);
	sem_post(&sem_file);
	sem_destroy(&sem_alloc);
	sem_destroy(&sem_file);
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
