#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "up_down.h"

#define INITIAL 10000
#define FINAL 1000

typedef struct
{
    int initial_vector[INITIAL];
    int final_vector[FINAL];
    int max;
} Vectors;

int main()
{

    // Criar a memória partilhada
    int data_size = sizeof(Vectors);
    int fd = shm_open("/shm_test", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(fd, data_size);
    Vectors *vector = (Vectors *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    vector->max = 0;

    // Criar e inicializar os semáforos
    sem_t *sem_max;
    sem_t *sem_avg1;
    sem_t *sem_avg2;
    sem_t *sem_avg3;
    sem_t *sem_avg4;
    sem_t *sem_avg5;

    if ((sem_max = sem_open("/sem_max", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED)
    {
        perror("Error creating semaphore");
        exit(EXIT_FAILURE);
    }

    if ((sem_avg1 = sem_open("/sem_avg1", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED)
    {
        perror("Error creating semaphore");
        exit(EXIT_FAILURE);
    }

    if ((sem_avg2 = sem_open("/sem_avg2", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED)
    {
        perror("Error creating semaphore");
        exit(EXIT_FAILURE);
    }

    if ((sem_avg3 = sem_open("/sem_avg3", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED)
    {
        perror("Error creating semaphore");
        exit(EXIT_FAILURE);
    }

    if ((sem_avg4 = sem_open("/sem_avg4", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED)
    {
        perror("Error creating semaphore");
        exit(EXIT_FAILURE);
    }

    if ((sem_avg5 = sem_open("/sem_avg5", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED)
    {
        perror("Error creating semaphore");
        exit(EXIT_FAILURE);
    }

    // Gerar uma nova seed
    srand(time(NULL) + getpid());

    // Preencher o vetor inicial com valores aleatórios
    for (int i = 0; i < INITIAL; i++)
    {
        vector->initial_vector[i] = rand() % 10000;
    }

    // Inicializar variáveis
    pid_t pid;
    int average;
    int adjustment = 0;

    // Criar os processos filhos
    for (int i = 0; i < 6; i++)
    {
        if ((pid = fork()) == -1)
        {
            perror("Error creating child process");
            exit(EXIT_FAILURE);
        }

        if (pid == 0)
        {
            switch (i)
            {
            case 0:
                for (int j = 0 + (i * 2000); j < 2000 + (i * 2000);
                     j += 10) // Incrementa 10 para ignorar os 10 já analisados pela leitura movida
                {
                    average = 0;
                    for (int k = j; k < j + 10; k++) // Cumpre o requisito de calcular a média movida de 10 valores (de 10 em 10)
                    {
                        average += vector->initial_vector[k];
                    }
                    average /= 10;

                    // Ajusta o índice para o vetor final a partir da variável do loop "j" (razão entre os tamanhos dos
                    // vetores 1/10)
                    adjustment = j / 10;

                    // Libertar o semáforo para a próxima média
                    up(sem_max);
                    vector->final_vector[adjustment] = average;
                    up(sem_avg2);
                }
                exit(EXIT_SUCCESS);
            case 1:
                for (int j = 0 + (i * 2000); j < 2000 + (i * 2000); j += 10)
                {
                    // Espera que a primeira média seja calculada e seja permitido calcular a segunda
                    down(sem_avg2);

                    average = 0;
                    for (int k = j; k < j + 10; k++)
                    {
                        average += vector->initial_vector[k];
                    }
                    average /= 10;

                    // Ajusta o índice para o vetor final a partir da variável do loop "j" (razão entre os tamanhos dos
                    // vetores 1/10)
                    adjustment = j / 10;

                    // Libertar o semáforo para a próxima média
                    up(sem_max);
                    vector->final_vector[adjustment] = average;
                    up(sem_avg3);
                }
                exit(EXIT_SUCCESS);
            case 2:
                for (int j = 0 + (i * 2000); j < 2000 + (i * 2000); j += 10)
                {
                    // Espera que a segunda média seja calculada e seja permitido calcular a terceira
                    down(sem_avg3);

                    average = 0;
                    for (int k = j; k < j + 10; k++)
                    {
                        average += vector->initial_vector[k];
                    }
                    average /= 10;

                    // Ajusta o índice para o vetor final a partir da variável do loop "j" (razão entre os tamanhos dos
                    // vetores 1/10)
                    adjustment = j / 10;

                    // Libertar o semáforo para a próxima média
                    up(sem_max);
                    vector->final_vector[adjustment] = average;
                    up(sem_avg4);
                }
                exit(EXIT_SUCCESS);
            case 3:
                for (int j = 0 + (i * 2000); j < 2000 + (i * 2000); j += 10)
                {
                    // Espera que a terceira média seja calculada e seja permitido calcular a quarta
                    down(sem_avg4);

                    average = 0;
                    for (int k = j; k < j + 10; k++)
                    {
                        average += vector->initial_vector[k];
                    }
                    average /= 10;

                    // Ajusta o índice para o vetor final a partir da variável do loop "j" (razão entre os tamanhos dos
                    // vetores 1/10)
                    adjustment = j / 10;

                    // Libertar o semáforo para a próxima média
                    up(sem_max);
                    vector->final_vector[adjustment] = average;
                    up(sem_avg5);
                }
                exit(EXIT_SUCCESS);
            case 4:
                for (int j = 0 + (i * 2000); j < 2000 + (i * 2000); j += 10)
                {
                    // Espera que a quarta média seja calculada e seja permitido calcular a quinta
                    down(sem_avg5);

                    average = 0;
                    for (int k = j; k < j + 10; k++)
                    {
                        average += vector->initial_vector[k];
                    }
                    average /= 10;

                    // Ajusta o índice para o vetor final a partir da variável do loop "j" (razão entre os tamanhos dos
                    // vetores 1/10)
                    adjustment = j / 10;

                    // Libertar o semáforo para a próxima média
                    up(sem_max);
                    vector->final_vector[adjustment] = average;
                    up(sem_avg1);
                }
                exit(EXIT_SUCCESS);
            case 5:
                for (int j = 0; j < FINAL; j++)
                {
                    down(sem_max);
                    if (vector->final_vector[j] > vector->max)
                    {
                        vector->max = vector->final_vector[j];
                        printf("New larger value found: %d\n", vector->max);
                    }
                }
                exit(EXIT_SUCCESS);
            }
        }
    }

    // Processo pai espera pelos filhos
    for (int i = 0; i < 6; i++)
    {
        wait(NULL);
    }

    // REQUISITO: Imprimir o vetor final e o valor máximo encontrado
    printf("\nFinal vector:\n");
    for (int i = 0; i < FINAL; i++)
    {
        printf("Final[%d] - %d\n", i, vector->final_vector[i]);
    }
    printf("\n");
    printf("Max value: %d\n", vector->max);

    // Libertar memória partilhada
    munmap(vector, data_size);
    close(fd);
    shm_unlink("/shm_test");

    // Libertar os semáforos
    sem_close(sem_avg1);
    sem_unlink("/sem_avg1");
    sem_close(sem_avg2);
    sem_unlink("/sem_avg2");
    sem_close(sem_avg3);
    sem_unlink("/sem_avg3");
    sem_close(sem_avg4);
    sem_unlink("/sem_avg4");
    sem_close(sem_avg5);
    sem_unlink("/sem_avg5");
    sem_close(sem_max);
    sem_unlink("/sem_max");

    return 0;
}
