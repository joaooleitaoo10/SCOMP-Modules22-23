#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define N_PRINTABLES 100
#define N_CHILDS 10

int main()
{
	// Create the array for the childs that will be created
	pid_t childs_created[N_CHILDS];

	// Loop to print 100 per childs
	for (int i = 0; i < N_CHILDS; i++) {
		// Creating the child
		pid_t pid = fork();
		childs_created[i] = pid;

		// Checking if it is the child
		if (pid == 0) {
			// Determining what's the first number to print
			int first_number = i * N_PRINTABLES + 1;
			int printable;
			for (int j = 0; j < 100; j++) {
				printable = first_number + j;
				printf("%d\n", printable);
			}
			exit(0);
		}
	}

	for (int i = 0; i < 10; i++) {
		waitpid(childs_created[i], NULL, 0);
	}

	return 0;
}

