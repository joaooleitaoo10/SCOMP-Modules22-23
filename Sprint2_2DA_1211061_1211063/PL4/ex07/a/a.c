#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>

// Padrões de comunicação entre processos: Sincronização de Execução
int main()
{
    // Declare the necessary variables
    pid_t pid1;
    sem_t *sem_beer, *sem_chips;

    // Create the semaphores for the chips and for the beer
    sem_beer = sem_open("sem1", O_CREAT | O_EXCL, 0644, 0);
    sem_chips = sem_open("sem2", O_CREAT | O_EXCL, 0644, 0);

    // Process 2
    pid1 = fork();
    if (pid1 == 0)
    {
        // Sleep random time between 0 and 5 seconds
        srand(getpid());
        sleep(rand() % 6);
        printf("P2: Just bought beer!\n");
        sem_post(sem_beer);
        sem_wait(sem_chips);
        printf("P2: Eating and drinking!\n");
        exit(EXIT_SUCCESS);
    }

    // Process 1
    // Sleep random time between 0 and 2 seconds
    srand(getpid());
    sleep(rand() % 3);
    printf("P1: Just bought chips!\n");
    sem_post(sem_chips);
    sem_wait(sem_beer);
    printf("P1: Eating and drinking!\n");

    // Wait for child processe to complete
    wait(NULL);

    // Close and unlink the semaphores
    sem_close(sem_beer);
    sem_close(sem_chips);
    sem_unlink("sem1");
    sem_unlink("sem2");

    return 0;
}