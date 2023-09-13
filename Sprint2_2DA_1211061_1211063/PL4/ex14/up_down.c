#include <stdio.h> 
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>

void up(sem_t *sem){
    // Incrementa o valor do semáforo
    if(sem_post(sem) == -1)
    {
        perror("Error incrementing the semaphore!\n");
        exit(EXIT_FAILURE);
    }
}

void down(sem_t *sem){
    // Se o valor for 1, decrementa para 0 e continua, se o valor for 0, espera
    if(sem_wait(sem) == -1)
    {
        perror("Error decrementing the semaphore!\n");
        exit(EXIT_FAILURE);
    }
}