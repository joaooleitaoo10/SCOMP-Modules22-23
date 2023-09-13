#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#include "up_down.h"

#define NUM_INTEGERS 10
#define NUM_AX_PROCESSES 3
#define NUM_BX_PROCESSES 2
#define NUM_PROCESSES 5

typedef struct
{
    int numbers[NUM_INTEGERS];
    int ax_num_execs;
    int bx_num_execs;
} SharedMemory;

int main()
{

    // Abrir a memória partilhada
    int data_size = sizeof(SharedMemory);
    int fd = shm_open("/shm_test", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(fd, data_size);
    SharedMemory *shared_memory = (SharedMemory *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Criar os semáforos
    sem_t *mutex_numbers;   // Permite o acesso por vários processos ao array de inteiros
    sem_t *mutex_num_execs; // Permite o acesso por vários processos ao número de execuções
    sem_t *sem_barrier;     // Barreira que permite que os processos ax e bx sejam executados alternadamente

    if ((mutex_numbers = sem_open("/mutex_numbers", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
    {
        perror("Error at sem_open()!\n");
        exit(EXIT_FAILURE);
    }

    if ((mutex_num_execs = sem_open("/mutex_num_execs", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
    {
        perror("Error at sem_open()!\n");
        exit(EXIT_FAILURE);
    }

    if ((sem_barrier = sem_open("/sem_barrier", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED)
    {
        perror("Error at sem_open()!\n");
        exit(EXIT_FAILURE);
    }

    // Inicializar as variáveis
    pid_t pid;
    shared_memory->ax_num_execs = 0;
    shared_memory->bx_num_execs = 0;

    for (int i = 0; i < 5; i++)
    {
        if ((pid = fork()) == -1)
        {
            perror("Error at fork()!\n");
            exit(EXIT_FAILURE);
        }
        if (pid == 0)
        {
            if ((i + 1) <= 3)
            {
                // Espera que o mutex dos números esteja livre
                down(mutex_numbers);
                printf("AX Wrote: ");
                for (int j = 0; j < NUM_INTEGERS; j++)
                {
                    shared_memory->numbers[j] = rand() % 100;
                    printf("%d ", shared_memory->numbers[j]);
                }
                printf("\n");

                sleep(5);

                // Liberta o mutex dos números
                up(mutex_numbers);
                down(mutex_num_execs);

                // Incrementa o número de execuções de Ax
                shared_memory->ax_num_execs++;

                // Se terminarem as 3 execuções de Ax, levanta a barreira
                if (shared_memory->ax_num_execs == 3)
                {
                    // Liberta o semáforo da barreira
                    up(sem_barrier);
                }

                // Liberta o mutex do número de execuções
                up(mutex_num_execs);
            }
            else if ((i + 1) > 3)
            {
                // Espera que o semáforo da barreira seja libertado
                down(sem_barrier);

                // Liberta a barreira para que o último Bx possa ser executado
                up(sem_barrier);

                // Espera que o mutex dos números esteja livre
                down(mutex_numbers);

                // Escreve os números
                printf("BX Wrote: ");
                for (int j = 0; j < NUM_INTEGERS; j++)
                {
                    shared_memory->numbers[j] = rand() % 100;
                    printf("%d ", shared_memory->numbers[j]);
                }
                printf("\n");

                // Dorme 6 segundos após escrever os números
                sleep(6);

                // Liberta o mutex dos números
                up(mutex_numbers);
            }

            exit(EXIT_SUCCESS);
        }
    }

    // Esperar que os processos filhos terminem
    for (int i = 0; i < 5; i++)
    {
        wait(NULL);
    }

    // Fechar a memória partilhada
    munmap(shared_memory, data_size);
    close(fd);
    shm_unlink("/shm_test");

    // Fechar os semáforos
    sem_close(mutex_numbers);
    sem_unlink("/mutex_numbers");
    sem_close(mutex_num_execs);
    sem_unlink("/mutex_num_execs");
    sem_close(sem_barrier);
    sem_unlink("/sem_barrier");

    return 0;
}
