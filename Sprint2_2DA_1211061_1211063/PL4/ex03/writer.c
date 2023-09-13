#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

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
    int fd = shm_open("/shm_test", O_CREAT | O_EXCL | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(fd, data_size);
    shared_data_type *shared_data = (shared_data_type *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Inicializações
    shared_data->line_occurrences = 0;
    pid_t get_pid = getpid();
    sem_init(&(shared_data->sem), 0, 1);

    // Escreve na memória partilhada
    srand(time(NULL));
    for (int i = 0; i < STRINGS; i++) {
        struct timespec timeout;
        timeout.tv_sec = 12; // REQUISITO: Definir os 12 segundos que vão ter de ser esperados
        timeout.tv_nsec = 0;

        int result = sem_timedwait(&(shared_data->sem), &timeout); // SUGESTÃO: Utilização da sugestão de utilizar a função sem_timedwait()
        if (result == -1) {
            printf("Timeout! Could not access shared memory. Exiting...\n");
            break;
        }

        int number = 0;

        int remove_last_string = (rand() % 10) < 3; // Forma de garantir uma probabilidade de 30% de remover a última string escrita
        if (remove_last_string && shared_data->line_occurrences > 0) {
            shared_data->line_occurrences--;
            printf("I'm the Father - with PID %d. Removed the last written string. Line occurrences: %d NE: %d\n", getpid(), shared_data->line_occurrences, number);
            number++;
        } else {
            for (int j = 0; j < CHARACTERS; j++) {
                if (shared_data->array[i][j] == '\0') {
                    sprintf(shared_data->array[i], "PID: %d - Line: %d\n", get_pid, i);
                    printf("I'm the Writer - with PID %d. Written a new string. Line occurrences: %d NE: %d\n", getpid(), shared_data->line_occurrences + 1, number);
                    shared_data->line_occurrences++;
                    number++;
                    break;
                }
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
