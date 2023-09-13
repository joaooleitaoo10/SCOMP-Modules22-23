#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/shm.h>

#define SEM_SELLER_AVAILIBITY_NAME "/sem_seller_availibility"
#define SEM_TICKET_BOUGHT_NAME "/sem_ticket_bought"
#define TICKETS_SHM_NAME "/tickets_shm"
#define N_CLIENTS 25

typedef struct
{
    int ticketNumber;
} Tickets;

int main()
{

    // Open the semaphores
    sem_t *sem_seller_availibility = sem_open(SEM_SELLER_AVAILIBITY_NAME, O_RDWR);
    if (sem_seller_availibility == SEM_FAILED)
    {
        printf("Error: Client side app couldn't open the seller availibity semaphore!\nTry running the seller side app first.\n");
        exit(1);
    }
    sem_t *sem_ticket_bought = sem_open(SEM_TICKET_BOUGHT_NAME, O_RDWR);
    if (sem_ticket_bought == SEM_FAILED)
    {
        printf("Error: Client side app couldn't open the ticket bought semaphore!\nTry running the seller side app first.\n");
        exit(1);
    }

    // Create the shared memory for the tickets
    int data_size = sizeof(Tickets);

    int fd = shm_open(TICKETS_SHM_NAME, O_RDONLY, S_IRUSR);
    if (fd == -1)
    {
        printf("Error: Client side app couldn't create shared memory area!\nTry running the seller side app first.\n");
        exit(1);
    }
    // if (ftruncate(fd, data_size) == -1)
    //{
    //     printf("Error: Client side app couldn't truncate shared memory area!\nTry running the seller side app first.\n");
    //     exit(1);
    // }
    Tickets *tickets = (Tickets *)mmap(NULL, data_size, PROT_READ, MAP_SHARED, fd, 0);
    if (tickets == MAP_FAILED)
    {
        printf("Error: Client side app couldn't map shared memory area!\nTry running the seller side app first.\n");
        exit(1);
    }

    // Create the actual clients
    for (int i = 0; i < N_CLIENTS; i++)
    {
        pid_t pid = fork();

        if (pid == 0)
        {
            // Wait a random amount of time from 1 - 10 seconds (time to be served)
            srand(time(NULL) * getpid() * rand() / 3 * getpid() * 987657755 * rand() * rand());
            int waitTime = rand() % 10 + 1;
            sleep(waitTime);

            // Wait for the seller to be available
            sem_wait(sem_seller_availibility);

            // Buy the ticket
            printf("\033[0;35mClient %d bought ticket %d\n\033[0;33m", getpid(), tickets->ticketNumber);

            // Signal the seller that the ticket was bought
            sem_post(sem_ticket_bought);

            exit(EXIT_SUCCESS);
        }
    }

    // Wait for all the clients to finish
    for (int i = 0; i < N_CLIENTS; i++)
    {
        wait(NULL);
    }

    // Unmap the shared memory
    munmap(tickets, data_size);

    return 0;
}