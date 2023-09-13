#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/shm.h>

int main(int argc, char *argv[])
{

    // Assure that the user has provided the correct number of arguments
    if (argc != 3)
    {
        printf("Usage: %s {nProcesses} {timeOfWork}\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Split the arguments
    int nProcesses = atoi(argv[1]);
    int timeOfWork = atoi(argv[2]);

    printf("\033[0;31mNumber of processes: %d --- Time of the work: %d \033[0;37m\n", nProcesses, timeOfWork);

    struct timespec start_time, end_time;
    long long execution_time;

    // Start time
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    pid_t pid;
    // Create the worker processes
    for (int i = 0; i < nProcesses; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            break;
        }
    }

    if (pid == 0)
    {
        printf("\033[0;35mProcess %d: Im working!!!\n\033[0;37m", getpid());
        usleep(timeOfWork); // If the goal is to split the work between the processes, add  " / nProcesses"
        printf("\033[0;32mProcess %d: Im done!!!\n\033[0;37m", getpid());
        exit(EXIT_SUCCESS);
    }

    for (int i = 0; i < nProcesses; i++)
    {
        wait(NULL);
    }

    // End time
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    // Calculate execution time
    execution_time = (end_time.tv_sec - start_time.tv_sec) * 1000000;
    execution_time += (end_time.tv_nsec - start_time.tv_nsec) / 1000;
    // Print the execution time
    printf("\033[0;34mExecution time: %.2lld milliseconds\n\033[0;37m", execution_time);

    return 0;
}