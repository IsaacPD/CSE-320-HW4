#define _GNU_SOURCE
#include "includes.h"

int main(int argc, char** argv){
	char* args[6];
	char buf[256];
	pid_t pid;

	printf("Type help for a list of commands\n");
	while(1){
		printf("prompt> ");
		fgets(buf, 256, stdin);
		buf[strlen(buf) - 1] = '\0';
		int len = strlen(buf);

		if (strcmp(buf, "exit") == 0){
			return 0;
		} else if (strcmp(buf, "help") == 0){
			printf("exit - quits application\nhelp - prints this message\nrun X - execute application X\n");
		} else if (strncmp(buf, "run", 3) == 0){
			int i = 3;
		   	int j = 0;
			for (i; i < len; i++){
				if (buf[i] == ' '){
					buf[i] = '\0';
					args[j] = &buf[i+1];
					j++;
				}
			}
			args[j] = NULL;
			if ((pid = fork()) == 0){
				execvp(args[0], args);
				char temp[sizeof(args[0]) + 2];
				strcat(temp, "./");
				strcat(temp, args[0]);
				args[0] = temp;
				execvp(args[0], args);
				perror("Execute");
				exit(0);
			} else {
				waitpid(pid, NULL, 0);
			}
		} else {
			printf("command not supported\n");
		}
	}
}
