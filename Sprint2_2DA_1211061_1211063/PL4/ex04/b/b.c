#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>

#define N_EXECUTIONS 15

// Padrões de comunicação entre processos: Sincronização de Execução
int main()
{
    // Declare the necessary variables
    pid_t pid1, pid2;
    sem_t *sem1, *sem2, *sem3;

    // Create the semaphores
    sem1 = sem_open("sem1", O_CREAT | O_EXCL, 0644, 1);
    sem2 = sem_open("sem2", O_CREAT | O_EXCL, 0644, 0);
    sem3 = sem_open("sem3", O_CREAT | O_EXCL, 0644, 0);

    // Check if the semaphores were created successfully
    if (sem1 == SEM_FAILED || sem2 == SEM_FAILED || sem3 == SEM_FAILED)
    {
        perror("Error creating the semaphores");
        exit(EXIT_FAILURE);
    }

    // Prints 1st child
    pid1 = fork();
    if (pid1 == 0)
    {
        for (int i = 0; i < N_EXECUTIONS; i++)
        {
            sem_wait(sem1);
            printf("1st child\n");
            fflush(stdout);
            sem_post(sem2);
        }
        exit(EXIT_SUCCESS);
    }

    // Prints 2nd child
    pid2 = fork();
    if (pid2 == 0)
    {
        for (int i = 0; i < N_EXECUTIONS; i++)
        {
            sem_wait(sem3);
            printf("2nd child\n\n");
            fflush(stdout);
            sem_post(sem1);
        }
        exit(EXIT_SUCCESS);
    }

    int counter = 0;
    for (int i = 0; i < N_EXECUTIONS; i++)
    {
        // Prints Father
        sem_wait(sem2);
        printf("Father \n");
        fflush(stdout);
        counter++;
        sem_post(sem3);
    }

    // Wait for child processes to complete
    for (int i = 0; i < 2; i++)
        wait(NULL);

    printf("Combination printed %d times\n", counter);

    // Close and unlink the semaphores
    sem_close(sem1);
    sem_close(sem2);
    sem_close(sem3);
    sem_unlink("sem1");
    sem_unlink("sem2");
    sem_unlink("sem3");

    return 0;
}