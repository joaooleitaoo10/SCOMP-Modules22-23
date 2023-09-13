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
#define N_TICKETS 25

typedef struct
{
    int ticketNumber;
} Tickets;

int main()
{

    // Creating the semaphore
    sem_t *sem_seller_availibility = sem_open(SEM_SELLER_AVAILIBITY_NAME, O_CREAT | O_EXCL, 0644, 0);
    if (sem_seller_availibility == SEM_FAILED)
    {
        printf("Error: The seller couldn't create the the seller availibity semaphore!\n");
        exit(1);
    }
    sem_t *sem_ticket_bought = sem_open(SEM_TICKET_BOUGHT_NAME, O_CREAT | O_EXCL, 0644, 0);
    if (sem_ticket_bought == SEM_FAILED)
    {
        printf("Error: The seller couldn't create the ticket bought semaphore!\n");
        exit(1);
    }

    // Create the shared memory for the tickets
    int data_size = sizeof(Tickets);
    int fd = shm_open(TICKETS_SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        printf("Error: Seller couldn't create shared memory area!\n");
        exit(1);
    }
    ftruncate(fd, data_size);
    if (ftruncate(fd, data_size) == -1)
    {
        printf("Error: Seller couldn't truncate shared memory area!\n");
        exit(1);
    }
    Tickets *tickets = (Tickets *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (tickets == MAP_FAILED)
    {
        printf("Error: Seller couldn't map shared memory area!\n");
        exit(1);
    }

    printf("\033[0;32m### SELLER: Tickets are now available for sale! ###\033[0;37m\n");

    // Selling tickets until there are no more tickets
    for (int i = 1; i <= N_TICKETS; i++)
    {
        tickets->ticketNumber = i;

        // Notify the buyer that there is a ticket available
        sem_post(sem_seller_availibility);
        // Wait for the buyer to buy the ticket
        sem_wait(sem_ticket_bought);
    }

    // Tickets ended
    printf("\033[0;33mm### SELLER: Tickets are now sold out! Come back next time!\n\033[0;37m ###");

    // Close the semaphores
    sem_close(sem_seller_availibility);
    sem_close(sem_ticket_bought);

    // Unlink the semaphores
    sem_unlink(SEM_SELLER_AVAILIBITY_NAME);
    sem_unlink(SEM_TICKET_BOUGHT_NAME);

    // Unlink the shared memory
    shm_unlink(TICKETS_SHM_NAME);

    return 0;
}