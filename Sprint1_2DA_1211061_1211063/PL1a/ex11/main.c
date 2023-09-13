#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define ARRAY_SIZE 1000
#define N_CHILDS 5

int main()
{
	// Create an array of 1000 random numbers
	int numbers[ARRAY_SIZE];
	time_t timeToRandom;
	srand((unsigned)time(&timeToRandom));
	for (int i = 0; i < ARRAY_SIZE; i++) {
		numbers[i] = rand() % 256;
	}

	pid_t childs_created[N_CHILDS];

	// a) create 5 child processes that will concurrently find the maximum value of 1/5 of the array;
	for (int i = 0; i < N_CHILDS; i++) {
		childs_created[i] = fork();
		int maximum = 0;

		if (childs_created[i] == 0) {
			for (int j = 0; j < ARRAY_SIZE / 5 * (i + 1); j++) {
				if (numbers[j] > maximum) {
					maximum = numbers[j];
				}
			}
			exit(maximum);
		}
	}

	int array_max = 0;
	int status;
	// Computing the maximum value of the array:
	for (int i = 0; i < N_CHILDS; i++) {
		waitpid(childs_created[i], &status, 0);

		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status) > array_max) {
				array_max = WEXITSTATUS(status);
			}
		}
	}

	printf("max: %d\n", array_max);

	// b) after computing the maximum value of the entire array, the parent process should create one child process
	// to perform the following calculation result[i] = ((int)numbers[i] / max_value) * 100 on half of the
	// array and print the result;

	int result[N_CHILDS];

	pid_t b_child = fork();
	if (b_child == 0) {
		for (int i = 0; i < ARRAY_SIZE / 2; i++) {
			result[i] = ((int)numbers[i] / array_max) * 100;
			printf("Index n%d: %d\n", i, result[i]);
		}
		exit(0);
	}

	// c) Perform the same operation for the second half and d) both child and parent process must perform the computation concurrently, but the output must be sorted by
	// the array indexes.
	for (int i = ARRAY_SIZE / 2; i < ARRAY_SIZE; i++) {
		result[i] = ((int)numbers[i] / array_max) * 100;
	}

	waitpid(b_child, &status, 0);

	if(WIFEXITED(status)){
		for (int i = ARRAY_SIZE / 2; i < ARRAY_SIZE; i++) {
			printf("Index n%d: %d\n", i, result[i]);
		}
	}

	return 0;
}

