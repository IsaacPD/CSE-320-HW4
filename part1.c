#define _GNU_SOURCE
#include "includes.h"

int main(int argc, char** argv){
	char* args[2];
	args[1] = NULL;
	char buf[256];
	pid_t pid;

	printf("Type help for a list of commands\n");
	while(1){
		printf("prompt> ");
		fgets(buf, 256, stdin);
		buf[strlen(buf) - 1] = '\0';

		if (strcmp(buf, "exit") == 0){
			return 0;
		} else if (strcmp(buf, "help") == 0){
			printf("exit - quits application\nhelp - prints this message\nrun X - execute application X\n");
		} else if (strncmp(buf, "run", 3) == 0){
			args[0] = &buf[4];
			if ((pid = fork()) == 0){
				execvp(args[0], args);
				exit(0);
			} else {
				waitpid(pid, NULL, 0);
			}
		} else {
			printf("command not supported\n");
		}
	}
}
