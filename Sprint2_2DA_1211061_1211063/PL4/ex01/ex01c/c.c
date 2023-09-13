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
    sem_t *semR[N_CHILDREN];
    for (int i = 0; i < N_CHILDREN; i++)
    {
        char name[20];
        sprintf(name, "semRead%d", i);
        semR[i] = sem_open(name, O_CREAT | O_EXCL, 0644, 0);
    }

    sem_t *semW = sem_open("semWrite", O_CREAT | O_EXCL, 0644, 1);

    pid_t pid[N_CHILDREN];
    // Create the children
    for (int i = 0; i < N_CHILDREN; i++)
    {
        pid[i] = fork();
        if (pid[i] == 0)
        {
            // Open the input file
            FILE *in = fopen(IN_FILE, "r");
            // get the pid value
            pid_t pidToPrint = getpid();

            // Wait for the semaphore corresponding to this child to be released
            sem_wait(semR[i]);

            // Read all numbers that belong to this child
            for (int j = 0; j < N_NUMBERS; j++)
            {
                int number;
                fscanf(in, "%d", &number);

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

            // Allow the next child to read
            sem_post(semR[(i + 1) % N_CHILDREN]);

            // Close the input file
            fclose(in);
            printf("Child %d finished\n", getpid());
            exit(0);
        }
    }

    // Release the first semaphore to start the processing
    sem_post(semR[0]);

    // Wait for the children to finish
    for (int i = 0; i < N_CHILDREN; i++)
    {
        wait(NULL);
    }

    // Destroy the semaphores
    for (int i = 0; i < N_CHILDREN; i++)
    {
        char name[20];
        sprintf(name, "semRead%d", i);
        sem_unlink(name);
    }
    sem_unlink("semWrite");

    // Print the output file
    FILE *out = fopen(OUT_FILE, "r");
    char line[100];
    while (fgets(line, 100, out) != NULL)
    {
        printf("%s", line);
    }

    return 0;
}
