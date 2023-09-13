#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define ARRAY_SIZE 1000

int main()
{
	int numbers[ARRAY_SIZE];	/* array to lookup */
	int n;			/* the number to find */
	time_t t;			/* needed to initialize random number generator (RNG) */
	int i;

	/* intializes RNG (srand():stdlib.h; time(): time.h) */
	srand((unsigned)time(&t));

	/* initialize array with random numbers (rand(): stdlib.h) */
	for (i = 0; i < ARRAY_SIZE; i++)
		numbers[i] = rand() % 10000;

	/* initialize n */
	n = rand() % 10000;

	int total = 0;

	// Se for o filho fazer a primeira metade, se for o pai fazer a segunda
	// Somar os resultados
	pid_t pid = fork();
	if (pid < 0) {
		exit(-1);
	}
	else if (pid == 0) {
		for (i = 0; i < ARRAY_SIZE / 2; i++) {
			if (numbers[i] == n) {
				total++;
			}
		}
		exit(total);
	}

	for (i = ARRAY_SIZE / 2; i < ARRAY_SIZE; i++) {
		if (numbers[i] == n) {
			total++;
		}
	}


	int childStatus;
	waitpid(pid, &childStatus, 0);
	if (WIFEXITED(childStatus)) {
		total += WEXITSTATUS(childStatus);
	}

	printf("There are %d %d's in the array\n", total, n);

	return 0;
}

