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

int insert(){

    // Abrir a memória partilhada
    int data_size = sizeof(record_logs);
    int fd = shm_open("/shmtest", O_CREAT | O_RDWR,S_IRUSR | S_IWUSR);
    ftruncate(fd, data_size);
    record_logs *ptr = (record_logs*)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
     
    // Inicializar o semáforo
    sem_t *sem;
    if((sem = sem_open("/sem_ex08_insert", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED){
        perror("Error at sem_open().\n");
        exit(EXIT_FAILURE);
    }

    // Esperar pelo semáforo
    down(sem);

    size_t buffer_size = 255;
    char *buffer = calloc(buffer_size, sizeof(char));

    // Inserir um novo registo
    printf("\nUSER'S IDENTIFICATION NUMBER: %d\n", ptr->identification_number+1); // Número que permite identificar um registo

    printf("INSERT NAME: ");
    getline(&buffer, &buffer_size, stdin);
    strncpy(ptr->records[ptr->identification_number].name, buffer, NAME_SIZE-1); // Copiar o conteúdo do buffer para a struct
    ptr->records[ptr->identification_number].name[strlen(ptr->records[ptr->identification_number].name) - 1] = '\0'; // Colocar o "\0" no final da string

    printf("INSERT ADDRESS: ");
    getline(&buffer, &buffer_size, stdin);
    ptr->records[ptr->identification_number].number = ptr->identification_number;
    strncpy(ptr->records[ptr->identification_number].address, buffer, ADDRESS_SIZE-1); // Copiar o conteúdo do buffer para a struct
    ptr->records[ptr->identification_number].address[strlen(ptr->records[ptr->identification_number].address) - 1] = '\0'; // Colocar o "\0" no final da string

    printf("INSERT NUMBER: ");
    scanf("%d%*c", &ptr->records[ptr->identification_number].number);

    ptr->identification_number++; // Incrementar o identificador de registos

    // Libertar o semáforo
    up(sem);

    // Fechar a memória partilhada
    munmap(ptr, data_size);
    close(fd);

    // Fechar o semáforo
    sem_close(sem);
    sem_unlink("/sem_ex08_insert");

    return 0;
}