#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/shm.h>

#define NUM_PROCESSES 6
typedef struct
{
    int barrier_procs;
} ShmData;

// Declare the necessary variables
sem_t *sem_nproc, *sem_barrier;
ShmData *shm;

// Buy beer function
void buy_beer()
{
    // Sleep random time between 0 and 5 seconds
    srand(getpid());
    sleep(rand() % 6);
    printf("P%d: Just bought beer!\n", getpid());
}

// Buy chips function
void buy_chips()
{
    // Sleep random time between 0 and 2 seconds
    srand(getpid());
    sleep(rand() % 3);
    printf("P%d: Just bought chips!\n", getpid());
}

// Eat and drink function
void eat_and_drink()
{
    printf("P%d: Eating and drinking!\n", getpid());
}

// Barrier function
void barrier()
{
    // Increment the number of processes that have arrived at the barrier, atomically
    sem_wait(sem_nproc);
    shm->barrier_procs++;
    sem_post(sem_nproc);

    // Check if all processes have arrived at the barrier
    if (shm->barrier_procs == NUM_PROCESSES)
    {
        sem_post(sem_barrier);
    }
    sem_wait(sem_barrier);
    sem_post(sem_barrier);
}

// Padrão de sincronização: barreira
int main()
{
    pid_t pid;

    // Create the semaphores for the chips and for the beer
    sem_nproc = sem_open("semprocs", O_CREAT | O_EXCL, 0644, 1);
    sem_barrier = sem_open("sembarrier", O_CREAT | O_EXCL, 0644, 0);

    // Create shared memory area
    int data_size = sizeof(ShmData);
    int fd = shm_open("/shm_procs", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, data_size) == -1)
    {
        perror("Error calling ftruncate()");
        exit(EXIT_FAILURE);
    }
    shm = (ShmData *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED)
    {
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
    // END Create shared memory area

    // Initialize the number of processes that have arrived at the barrier
    shm->barrier_procs = 0;

    // Fork the processes
    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            // Randomly decide if the process buys beer or chips
            srand(getpid());
            int choice = rand() % 2;
            if (choice == 0)
            {
                buy_beer();
            }
            else
            {
                buy_chips();
            }

            // Wait for all processes to arrive at the barrier
            barrier();

            // Process eats and drinks
            eat_and_drink();

            exit(EXIT_SUCCESS);
        }
    }

    // Wait for all child processes to complete
    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        wait(NULL);
    }

    // Close and unlink the semaphores
    sem_close(sem_nproc);
    sem_unlink("semprocs");
    sem_close(sem_barrier);
    sem_unlink("sembarrier");

    // Close and unlink the shared memory
    close(fd);
    shm_unlink("/shm_procs");

    return 0;
}
