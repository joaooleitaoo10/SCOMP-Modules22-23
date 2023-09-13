#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {

	pid_t pid1, pid2;

	pid1 = fork();
	if (pid1 == 0) {
		sleep(1);
		exit(1);
	}
	else {
		pid2 = fork();
		if (pid2 == 0) {
			sleep(2);
			exit(2);
		}
		else {
			int status;
			waitpid(pid1, &status, 0);
			printf("Child 1, of id %d exited with status %d\n", pid1, WEXITSTATUS(status));
			waitpid(pid2, &status, 0);
			printf("Child 2, of id %d exited with status %d\n", pid2, WEXITSTATUS(status));	
		}
	}

	return 0;
}