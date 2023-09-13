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

    // Create the semaphores
    sem_t *semR = sem_open("semRead", O_CREAT | O_EXCL, 0644, 1);
    sem_t *semW = sem_open("semWrite", O_CREAT | O_EXCL, 0644, 1);

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
        // get the pid value
        pid_t pidToPrint = getpid();
        for (int i = 0; i < N_NUMBERS; i++)
        {
            // READ - Stop other children from reading
            sem_wait(semR);
            // Read a number and write it to the output file
            int number;
            fscanf(in, "%d", &number);
            // Allow other children to read
            sem_post(semR);
            // READ END

            // WRITE - Stop other children from writing
            sem_wait(semW);
            // Open the output file
            FILE *out = fopen(OUT_FILE, "a");
            // Write the number to the output file
            fprintf(out, "[%d]: %d\n", pidToPrint, number);
            // Close the output file
            fclose(out);
            // Allow other children to write
            sem_post(semW);
            // WRITE END
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
    // Destroy the semaphores
    sem_unlink("semWrite");
    sem_unlink("semRead");

    // Print the output file
    FILE *out = fopen(OUT_FILE, "r");
    char line[100];
    while (fgets(line, 100, out) != NULL)
    {
        printf("%s", line);
    }

    return 0;
}
