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
#include <stdbool.h>

#define VIP_QUEUE_MUTEX "/sem_vip_queue_mutex"
#define SPECIAL_QUEUE_MUTEX "/sem_special_queue_mutex"
#define NORMAL_QUEUE_MUTEX "/sem_normal_queue_mutex"
#define SEM_VIP_QUEUE "/sem_vip_queue"
#define SEM_SPECIAL_QUEUE "/sem_special_queue"
#define SEM_NORMAL_QUEUE "/sem_normal_queue"
#define SEM_VIP_QUEUE "/sem_vip_queue"
#define SEM_SPECIAL_QUEUE "/sem_special_queue"
#define SEM_NORMAL_QUEUE "/sem_normal_queue"
#define SEM_CLUB_ENTRIES "/sem_club_entry"
#define VIP_QUEUE_SHM_NAME "/vip_queue_shm"
#define SPECIAL_QUEUE_SHM_NAME "/special_queue_shm"
#define NORMAL_QUEUE_SHM_NAME "/normal_queue_shm"
#define CLUB_ENTRIES_SHM_NAME "/club_entries_shm"
#define CLUB_ENTRIES_MUTEX "/sem_club_entries_mutex"

#define N_CLIENTS 30
#define N_CLUB_CAPACITY 3
#define VIP_PROBABILITY 15
#define SPECIAL_PROBABILITY 25
#define NORMAL_PROBABILITY 60

#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define YELLOW "\033[0;33m"
#define PURPLE "\033[0;35m"

// Padrões: Sincronização de execução, acesso a recursos partilhados

typedef struct
{
    int totalClients;
} VipQueue;

typedef struct
{
    int totalClients;
} SpecialQueue;

typedef struct
{
    int totalClients;
} NormalQueue;

typedef struct
{
    int totalEntries;
} ClubEntries;

ClubEntries *clubEntries;
VipQueue *vipQueue;
SpecialQueue *specialQueue;
NormalQueue *normalQueue;
sem_t *sem_vip_queue;
sem_t *sem_special_queue;
sem_t *sem_normal_queue;
sem_t *sem_vip_queue_mutex;
sem_t *sem_special_queue_mutex;
sem_t *sem_normal_queue_mutex;
sem_t *sem_club_entries;
sem_t *sem_club_entries_mutex;

void initializeSharedMemory()
{
    // Create the shared memory for the club entries
    int data_size = sizeof(ClubEntries);
    int fd = shm_open(CLUB_ENTRIES_SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        printf("Error: Couldn't create the shared memory area\n");
        exit(1);
    }
    ftruncate(fd, data_size);
    if (ftruncate(fd, data_size) == -1)
    {
        printf("Error: Couldn't truncate shared memory area!\n");
        exit(1);
    }
    clubEntries = (ClubEntries *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (clubEntries == MAP_FAILED)
    {
        printf("Error: Couldn't map shared memory area!\n");
        exit(1);
    }
    clubEntries->totalEntries = 0;

    // Create the shared memory for the vip queue
    data_size = sizeof(VipQueue);
    fd = shm_open(VIP_QUEUE_SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        printf("Error: Couldn't create the shared memory area\n");
        exit(1);
    }
    ftruncate(fd, data_size);
    if (ftruncate(fd, data_size) == -1)
    {
        printf("Error: Couldn't truncate shared memory area!\n");
        exit(1);
    }
    vipQueue = (VipQueue *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (vipQueue == MAP_FAILED)
    {
        printf("Error: Couldn't map shared memory area!\n");
        exit(1);
    }
    vipQueue->totalClients = 0;
    // Create the shared memory for the special queue
    data_size = sizeof(SpecialQueue);
    fd = shm_open(SPECIAL_QUEUE_SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        printf("Error: Couldn't create the shared memory area\n");
        exit(1);
    }
    ftruncate(fd, data_size);
    if (ftruncate(fd, data_size) == -1)
    {
        printf("Error: Couldn't truncate shared memory area!\n");
        exit(1);
    }
    specialQueue = (SpecialQueue *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (specialQueue == MAP_FAILED)
    {
        printf("Error: Couldn't map shared memory area!\n");
        exit(1);
    }
    specialQueue->totalClients = 0;
    // Create the shared memory for the normal queue
    data_size = sizeof(NormalQueue);
    fd = shm_open(NORMAL_QUEUE_SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        printf("Error: Couldn't create the shared memory area\n");
        exit(1);
    }
    ftruncate(fd, data_size);
    if (ftruncate(fd, data_size) == -1)
    {
        printf("Error: Couldn't truncate shared memory area!\n");
        exit(1);
    }
    normalQueue = (NormalQueue *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (normalQueue == MAP_FAILED)
    {
        printf("Error: Couldn't map shared memory area!\n");
        exit(1);
    }
}

void initializeSemaphores()
{
    // Creating the semaphores
    sem_vip_queue = sem_open(SEM_VIP_QUEUE, O_CREAT | O_EXCL, 0644, 0);
    if (sem_vip_queue == SEM_FAILED)
    {
        printf("Error: Couldn't create the vip queue semaphore!\n");
        exit(1);
    }
    sem_special_queue = sem_open(SEM_SPECIAL_QUEUE, O_CREAT | O_EXCL, 0644, 0);
    if (sem_special_queue == SEM_FAILED)
    {
        printf("Error: Couldn't create the special queue semaphore!\n");
        exit(1);
    }
    sem_normal_queue = sem_open(SEM_NORMAL_QUEUE, O_CREAT | O_EXCL, 0644, 0);
    if (sem_normal_queue == SEM_FAILED)
    {
        printf("Error: Couldn't create the normal queue semaphore!\n");
        exit(1);
    }
    sem_club_entries = sem_open(SEM_CLUB_ENTRIES, O_CREAT | O_EXCL, 0644, N_CLUB_CAPACITY);
    if (sem_club_entries == SEM_FAILED)
    {
        printf("Error: Couldn't create the club entries semaphore!\n");
        exit(1);
    }
    sem_vip_queue_mutex = sem_open(VIP_QUEUE_MUTEX, O_CREAT | O_EXCL, 0644, 1);
    if (sem_vip_queue_mutex == SEM_FAILED)
    {
        printf("Error: Couldn't create the vip queue mutex semaphore!\n");
        exit(1);
    }
    sem_special_queue_mutex = sem_open(SPECIAL_QUEUE_MUTEX, O_CREAT | O_EXCL, 0644, 1);
    if (sem_special_queue_mutex == SEM_FAILED)
    {
        printf("Error: Couldn't create the special queue mutex semaphore!\n");
        exit(1);
    }
    sem_normal_queue_mutex = sem_open(NORMAL_QUEUE_MUTEX, O_CREAT | O_EXCL, 0644, 1);
    if (sem_normal_queue_mutex == SEM_FAILED)
    {
        printf("Error: Couldn't create the normal queue mutex semaphore!\n");
        exit(1);
    }
    sem_club_entries_mutex = sem_open(CLUB_ENTRIES_MUTEX, O_CREAT | O_EXCL, 0644, 1);
    if (sem_club_entries_mutex == SEM_FAILED)
    {
        printf("Error: Couldn't create the club entries mutex semaphore!\n");
        exit(1);
    }
}

void waitClientsAndClub()
{
    // Wait for the clients to finish
    for (int i = 0; i < N_CLIENTS + 1; i++)
    {
        wait(NULL);
    }
}

void cleanUp()
{
    // Close the semaphores
    sem_close(sem_vip_queue);
    sem_close(sem_special_queue);
    sem_close(sem_normal_queue);
    sem_close(sem_club_entries);
    sem_close(sem_vip_queue_mutex);
    sem_close(sem_special_queue_mutex);
    sem_close(sem_normal_queue_mutex);
    sem_close(sem_club_entries_mutex);

    // Unlink the semaphores
    sem_unlink(SEM_VIP_QUEUE);
    sem_unlink(SEM_SPECIAL_QUEUE);
    sem_unlink(SEM_NORMAL_QUEUE);
    sem_unlink(SEM_CLUB_ENTRIES);
    sem_unlink(VIP_QUEUE_MUTEX);
    sem_unlink(SPECIAL_QUEUE_MUTEX);
    sem_unlink(NORMAL_QUEUE_MUTEX);
    sem_unlink(CLUB_ENTRIES_MUTEX);

    // Unlink the shared memory
    shm_unlink(VIP_QUEUE_SHM_NAME);
    shm_unlink(SPECIAL_QUEUE_SHM_NAME);
    shm_unlink(NORMAL_QUEUE_SHM_NAME);
    shm_unlink(CLUB_ENTRIES_SHM_NAME);
}

void vipClient(int clientId, int timeInTheClub)
{
    // Increase the total number of clients in the vip queue
    sem_wait(sem_vip_queue_mutex);
    vipQueue->totalClients++;
    sem_post(sem_vip_queue_mutex);

    printf(GREEN "VIP: Client %d is waiting in the VIP queue - WAIT\n", clientId);
    fflush(stdout);

    // Wait in the vip queue
    sem_wait(sem_vip_queue);

    // Increase the total number of entries
    sem_wait(sem_club_entries_mutex);
    clubEntries->totalEntries++;
    sem_post(sem_club_entries_mutex);

    printf(GREEN "VIP: Vip client %d is entering the club - ENTER\n", clientId);
    fflush(stdout);

    // Wait for the client to be in the club
    sleep(timeInTheClub);

    printf(GREEN "VIP: Vip client %d is leaving the club - LEAVE\n", clientId);
    fflush(stdout);
}

void specialClient(int clientId, int timeInTheClub)
{
    // Increase the total number of clients in the special queue
    sem_wait(sem_special_queue_mutex);
    specialQueue->totalClients++;
    sem_post(sem_special_queue_mutex);

    printf(BLUE "SPECIAL: Client %d is waiting in the SPECIAL queue - WAIT\n", clientId);
    fflush(stdout);

    // Wait in the special queue
    sem_wait(sem_special_queue);

    // Increase the total number of entries
    sem_wait(sem_club_entries_mutex);
    clubEntries->totalEntries++;
    sem_post(sem_club_entries_mutex);

    printf(BLUE "SPECIAL: Special client %d is entering the club - ENTER\n", clientId);
    fflush(stdout);

    // Wait for the client to be in the club
    sleep(timeInTheClub);

    printf(BLUE "SPECIAL: Special client %d is leaving the club - LEAVE\n", clientId);
    fflush(stdout);
}

void normalClient(int clientId, int timeInTheClub)
{
    // Increase the total number of clients in the normal queue
    sem_wait(sem_normal_queue_mutex);
    normalQueue->totalClients++;
    sem_post(sem_normal_queue_mutex);

    printf(YELLOW "NORMAL: Client %d is waiting in the NORMAL queue - WAIT\n", clientId);
    fflush(stdout);

    // Wait in the normal queue
    sem_wait(sem_normal_queue);

    // Increase the total number of entries
    sem_wait(sem_club_entries_mutex);
    clubEntries->totalEntries++;
    sem_post(sem_club_entries_mutex);

    printf(YELLOW "NORMAL: Normal client %d is entering the club - ENTER\n", clientId);
    fflush(stdout);

    // Wait for the client to be in the club
    sleep(timeInTheClub);

    printf(YELLOW "NORMAL: Normal client %d is leaving the club - LEAVE\n", clientId);
    fflush(stdout);
}

void clientProcess(int clientId)
{
    // Generate a random number to determine the type of client
    srand(time(NULL) + clientId * getpid() * clientId * getpid() * rand() * rand() * 986776458 * getpid() / clientId);
    int clientType = rand() % 100;

    // Generate a random number to determine the time the client will be in the club (3 - 5 seconds)
    int timeInTheClub = rand() % 3 + 3;

    // Execute the client
    if (clientType < VIP_PROBABILITY)
    {
        vipClient(clientId, timeInTheClub);
    }
    else if (clientType < VIP_PROBABILITY + SPECIAL_PROBABILITY)
    {
        specialClient(clientId, timeInTheClub);
    }
    else
    {
        normalClient(clientId, timeInTheClub);
    }

    // Release the club entry
    sem_post(sem_club_entries);
}

void createClients()
{
    // Create the clients
    for (int i = 1; i <= N_CLIENTS; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            // Execute the client
            clientProcess(i);
            exit(EXIT_SUCCESS);
        }
    }
}

void clubApp()
{
    printf(PURPLE " #### Club is open ####\n");
    fflush(stdout);
    int totalEntries, someoneEntered;
    while (true)
    {
        sem_wait(sem_club_entries_mutex);
        totalEntries = clubEntries->totalEntries;
        sem_post(sem_club_entries_mutex);
        if (totalEntries == N_CLIENTS)
        {
            break;
        }

        someoneEntered = 0;
        // Wait if the club is full
        sem_wait(sem_club_entries);
        // Check if there is a client in the vip queue
        int vipQueueSize = 0;
        // Wait for the vip queue to be available
        sem_wait(sem_vip_queue_mutex);
        vipQueueSize = vipQueue->totalClients;
        if (vipQueueSize > 0)
        {
            vipQueue->totalClients--;
            // Let the client enter the club
            sem_post(sem_vip_queue);
            totalEntries++;
            someoneEntered = 1;
        }
        sem_post(sem_vip_queue_mutex);
        if (someoneEntered == 0)
        {
            // Check if there is a client in the special queue
            int specialQueueSize = 0;
            // Wait for the special queue to be available
            sem_wait(sem_special_queue_mutex);
            specialQueueSize = specialQueue->totalClients;
            if (specialQueueSize > 0)
            {
                specialQueue->totalClients--;
                // Let the client enter the club
                sem_post(sem_special_queue);
                totalEntries++;
                someoneEntered = 1;
            }
            sem_post(sem_special_queue_mutex);

            if (someoneEntered == 0)
            {
                // Check if there is a client in the normal queue
                int normalQueueSize = 0;
                // Wait for the normal queue to be available
                sem_wait(sem_normal_queue_mutex);
                normalQueueSize = normalQueue->totalClients;

                if (normalQueueSize > 0)
                {
                    normalQueue->totalClients--;
                    // Let the client enter the club
                    sem_post(sem_normal_queue);
                    totalEntries++;
                    someoneEntered = 1;
                }
                sem_post(sem_normal_queue_mutex);

                if (someoneEntered == 0)
                {
                    // If no one entered, release the club entry
                    sem_post(sem_club_entries);
                }
            }
        }
    }
}

int main()
{
    initializeSharedMemory();
    initializeSemaphores();

    pid_t pid = fork();
    if (pid == 0)
    {
        // Execute the club
        clubApp();
        exit(EXIT_SUCCESS);
    }
    // Create the clients
    createClients();

    // Wait for the clients to finish
    waitClientsAndClub();

    // Print final message
    printf(PURPLE "### After %d clients, and a long night, the club is closing ###\n", N_CLIENTS);
    fflush(stdout);

    // Clean up the semaphores and shared memory
    cleanUp();

    return 0;
}