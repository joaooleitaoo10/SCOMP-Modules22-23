#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <string.h>

#define STRINGS 50
#define CHARACTERS 80

typedef struct{
    sem_t sem;
    char array[STRINGS][CHARACTERS];
    int line_occurences;
} shared_data_type;

int main(){

    // Cria e abre a memória partilhada
    int data_size = sizeof(shared_data_type);
    int fd = shm_open("/shm_test", O_CREAT | O_EXCL | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(fd, data_size);
    shared_data_type *shared_data = (shared_data_type*)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    //Inicializações
    shared_data->line_occurences = 0;
    pid_t get_pid = getpid(); 
    // Inicializa o semáforo com valor 1 para que este já se encontre disponível (O 0 significa que o semáforo é partilhado entre processos)
    sem_init(&(shared_data->sem), 0, 1); 

    // Escreve na memória partilhada
    for(int i = 0; i < STRINGS; i++){
        // Espera que o semáforo fique disponível
        sem_wait(&(shared_data->sem));

        for(int j = 0; j < CHARACTERS; j++){
            if (shared_data->array[i][j] == '\0'){
                sprintf(shared_data->array[i], "PID: %d - Line: %d", get_pid, i);
                printf("I'm the Father - with PID %d\n", getpid());
                shared_data->line_occurences++;
                break;
            }
        }

        // Liberta o semáforo
        sem_post(&(shared_data->sem));

        // Espera um tempo aleatório entre 1 e 5 segundos
        sleep(rand() % 6);
    }

    // Limpa a memória partilhada
    munmap(shared_data, data_size);
    close(fd);
    shm_unlink("/shm_test");

    return 0;
}