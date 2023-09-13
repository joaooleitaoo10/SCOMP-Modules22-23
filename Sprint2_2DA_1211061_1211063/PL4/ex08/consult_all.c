#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>

#include "header.h"

#define NAME_SIZE 50
#define ADDRESS_SIZE 100
#define MAX_RECORDS 100

typedef struct {
    int number;
    char name[NAME_SIZE];
    char address[ADDRESS_SIZE];
} record;

typedef struct {
    record records[MAX_RECORDS];
    int identification_number;
} record_logs;


int consultAll(){
    // Abrir a memória partilhada
    int data_size = sizeof(record_logs);
    int fd = shm_open("/shmtest", O_CREAT | O_RDWR,S_IRUSR | S_IWUSR);
    ftruncate(fd, data_size);
    record_logs *ptr = (record_logs*)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
     
    // Inicializar o semáforo
    sem_t *sem;
    if((sem = sem_open("/sem_ex08_consult_all", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED){
        perror("Error at sem_open().\n");
        exit(EXIT_FAILURE);
    }

    // Espera pelo semáforo
    down(sem);

    if(ptr->identification_number <=0){
        printf("\nThere are no records to consult!\n");
    }

    // Ler todos os registos
    for(int i = 0; i < ptr->identification_number; i++) // Percorre todos os records existentes
    {
            printf("\n\nRECORD DATA \n");
            printf("Record Identification Number: %d\n", i+1);
            printf("Name: %s\n", ptr->records[i].name);
            printf("Address: %s\n", ptr->records[i].address);
            printf("Number: %d\n\n", ptr->records[i].number);
            sleep(1);
    }

    // Libertar o semáforo
    up(sem);

    // Fechar a memória partilhada
    munmap(ptr, data_size);
    close(fd);

    // Fechar o semáforo
    sem_close(sem);
    sem_unlink("/sem_ex08_consult_all");

    return 0;
}