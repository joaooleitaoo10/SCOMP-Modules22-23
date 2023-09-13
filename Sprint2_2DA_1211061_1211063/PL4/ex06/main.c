#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>

void down(sem_t * sem)
{
    // Se o valor for 1, decrementa para 0 e continua, se o valor for 0, espera
    if(sem_wait(sem) == -1)
    {
        perror("Error decrementing the semaphore!\n");
        exit(EXIT_FAILURE);
    }
}

void up(sem_t * sem)
{
    // Incrementa o valor do semáforo
    if(sem_post(sem) == -1)
    {
        perror("Error incrementing the semaphore!\n");
        exit(EXIT_FAILURE);
    }
}

int main()
{

// Criação das variáveis
    sem_t *sem_nproc;
    sem_t *sem_barrier;
    pid_t pid;

// Inicialização das variáveis
        // Inicializam ambos com valor 2 porque esta ser o número máximo de "S" e "C" entre eles.
    sem_nproc = sem_open("sem_ex6_nproc", O_CREAT | O_EXCL, 0644, 2); 
    sem_barrier = sem_open("sem_ex6_barrier", O_CREAT | O_EXCL, 0644, 2);
    
    if((pid = fork()) == -1)
    {
        perror("Fork error!\n");
        exit(EXIT_FAILURE);
    }

    if(pid == 0) // Processo filho
    {
        while(1) // Cumprindo o exemplo do enunciado
        {
            down(sem_nproc);
            printf("C");
            fflush(stdout); // Seguindo a sugestão do enunciado
            up(sem_barrier);
        }
    } else { // Processo pai
        while(1)
        {
            down(sem_barrier);
            printf("S");
            fflush(stdout); // Seguindo a sugestão do enunciado
            up(sem_nproc);
        }
    }

    // Fechar os semáforos
    sem_close(sem_nproc);
    sem_unlink("sem_ex6_nproc");

    sem_close(sem_barrier);
    sem_unlink("sem_ex6_barrier");

    return 0;
}