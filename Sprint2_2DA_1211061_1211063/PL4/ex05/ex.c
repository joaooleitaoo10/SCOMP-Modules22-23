#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>

// Padrão de comunicação entre processos: Sincronização de Execução
int main()
{
    // Declare the necessary variables
    pid_t pid1, pid2, pid3;
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

    // 1st child (Sistemas ) + (a )
    pid1 = fork();
    if (pid1 == 0)
    {
        sem_wait(sem1);
        printf("Sistemas ");
        fflush(stdout);
        sem_post(sem2);
        sem_wait(sem1);
        printf("a ");
        fflush(stdout);
        sem_post(sem2);
        exit(EXIT_SUCCESS);
    }

    // 2nd child (de ) + (melhor )
    pid2 = fork();
    if (pid2 == 0)
    {
        sem_wait(sem2);
        printf("de ");
        fflush(stdout);
        sem_post(sem3);
        sem_wait(sem2);
        printf("melhor ");
        fflush(stdout);
        sem_post(sem3);
        exit(EXIT_SUCCESS);
    }

    // 3rd child (Computadores - ) + (disciplina! )
    pid3 = fork();
    if (pid3 == 0)
    {
        sem_wait(sem3);
        printf("Computadores - ");
        fflush(stdout);
        sem_post(sem1);
        sem_wait(sem3);
        printf("disciplina!\n");
        exit(EXIT_SUCCESS);
    }

    // Wait for child processes to complete
    for (int i = 0; i < 3; i++)
        wait(NULL);

    // Close and unlink the semaphores
    sem_close(sem1);
    sem_close(sem2);
    sem_close(sem3);
    sem_unlink("sem1");
    sem_unlink("sem2");
    sem_unlink("sem3");

    return 0;
}
