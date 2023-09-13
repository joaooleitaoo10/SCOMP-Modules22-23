#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>

#define OUT_FILE "Output.txt"
#define IN_FILE "Numbers.txt"
#define N_CHILDREN 8
#define N_NUMBERS 200

int main()
{
    // Remove the output file if it exists
    remove(OUT_FILE);

    // Create the semaphore
    sem_t *sem = sem_open("sem", O_CREAT | O_EXCL, 0644, 1);

    pid_t pid;
    // Create the children
    for (int i = 0; i < N_CHILDREN; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            break;
        }
    }

    if (pid == 0)
    {
        // Open the input file
        FILE *in = fopen(IN_FILE, "r");
        for (int i = 0; i < N_NUMBERS; i++)
        {
            sem_wait(sem);
            // get the pid value
            pid_t pidToPrint = getpid();

            // Open the output file
            FILE *out = fopen(OUT_FILE, "a");

            // Read a number and write it to the output file
            int number;
            fscanf(in, "%d", &number);
            fprintf(out, "[%d]: %d\n", pidToPrint, number);

            fclose(out);
            sem_post(sem);
        }
        // Close the input file
        fclose(in);
        printf("Child %d finished\n", getpid());
        exit(0);
    }

    // Wait for the children to finish
    for (int i = 0; i < N_CHILDREN; i++)
    {
        wait(NULL);
    }
    // Destroy the semaphore
    sem_unlink("sem");

    return 0;
}
