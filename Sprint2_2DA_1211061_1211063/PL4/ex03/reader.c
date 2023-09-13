#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>

#define STRINGS 50
#define CHARACTERS 80

typedef struct {
    sem_t sem;
    char array[STRINGS][CHARACTERS];
    int line_occurrences;
} shared_data_type;

int main() {

    // Cria e abre a memória partilhada
    int data_size = sizeof(shared_data_type);
    int fd = shm_open("/shm_test", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    shared_data_type *shared_data = (shared_data_type *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    for (int i = 0; i < STRINGS; i++) {
        // Print da informação lida da shared memory
        printf("Current number of line occurrences: %d\n", shared_data->line_occurrences);

        // Sleep durante um tempo aleatório entre 1 e 5 segundos
        sleep(rand() % 6);
    }

    printf("\nFinal number of line occurrences: %d\n", shared_data->line_occurrences);

    // Limpa a memória partilhada
    munmap(shared_data, data_size);
    close(fd);
    shm_unlink("/shm_test");

    return 0;
}
