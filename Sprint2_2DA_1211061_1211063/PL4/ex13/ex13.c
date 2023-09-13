#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define N_READERS 5
#define N_WRITERS 10

typedef struct
{
    int n_readers;
} Readers;

typedef struct
{
    int n_writers;
} Writers;

typedef struct
{
    char content[100000];
} ShmArea;

// N_Readers shared memory
Readers *n_readers;
// N_Writers shared memory
Writers *n_writers;
// Shared memory area where the readers and writers will write and read
ShmArea *shm_area;
// Semaphore to control the access to the number of readers
sem_t *sem_n_readers;
// Semaphore to control the access to the number of writers
sem_t *sem_n_writers;
// Semaphore to allow/disallow a reader to access its critical section (Used to assure only one reader accesses its critical section at a time)
sem_t *sem_reader_critical_section;
// Semaphore to allow/disallow a writer to access the shared memory area
sem_t *sem_writer;
// Semaphore to allow/disallow a reader to try to access its critical section (Used to prioritize writers)
sem_t *sem_reader;

// PADRÕES: Problema do leitor-escritor com prioridade para os escritores | Acesso exclusivo

// Method used to initialize the shared memory areas and semaphores
void initialiazeProgram()
{
    // Create the semaphores
    sem_n_readers = sem_open("sem_n_readers", O_CREAT | O_EXCL, 0644, 1);
    sem_n_writers = sem_open("sem_n_writers", O_CREAT | O_EXCL, 0644, 1);
    sem_reader_critical_section = sem_open("sem_reader_critical_section", O_CREAT | O_EXCL, 0644, 1);
    sem_writer = sem_open("sem_writer", O_CREAT | O_EXCL, 0644, 1);
    sem_reader = sem_open("sem_reader", O_CREAT | O_EXCL, 0644, 1);

    // Create the shared memory areas
    int data_size = sizeof(Readers);
    int fd = shm_open("n_readers", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
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
    n_readers = (Readers *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (n_readers == MAP_FAILED)
    {
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
    n_readers->n_readers = 0;

    data_size = sizeof(Writers);
    fd = shm_open("n_writers", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
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
    n_writers = (Writers *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (n_writers == MAP_FAILED)
    {
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
    n_writers->n_writers = 0;

    data_size = sizeof(ShmArea);
    fd = shm_open("shm_area", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
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
    shm_area = (ShmArea *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm_area == MAP_FAILED)
    {
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
    shm_area->content[0] = '\0';
}

// Method used to terminate the program, free the memory and semaphores
void terminateProgram()
{
    // Close the semaphores
    sem_close(sem_n_readers);
    sem_close(sem_n_writers);
    sem_close(sem_reader_critical_section);
    sem_close(sem_writer);
    sem_close(sem_reader);
    // Unlink the semaphores
    sem_unlink("sem_n_readers");
    sem_unlink("sem_n_writers");
    sem_unlink("sem_reader_critical_section");
    sem_unlink("sem_writer");
    sem_unlink("sem_reader");

    // Close and unlink the shared memory
    shm_unlink("n_readers");
    shm_unlink("n_writers");
    shm_unlink("shm_area");
}

// Method used to wait for the reader to be able to access its critical section
void sem_wait_critical_section()
{
    sem_wait(sem_reader_critical_section);
    sem_wait(sem_reader);
    sem_wait(sem_n_readers);
}

// Method used to post the reader to be able to access its critical section
void sem_post_critical_section()
{
    sem_post(sem_n_readers);
    sem_post(sem_reader);
    sem_post(sem_reader_critical_section);
}

void createReader()
{
    // Create a new process
    pid_t pid = fork();
    if (pid == 0)
    {
        sem_wait_critical_section();
        n_readers->n_readers++;
        if (n_readers->n_readers == 1)
        {
            sem_wait(sem_writer);
        }
        sem_post_critical_section();

        // Read the shared memory area
        char content_read[100000];
        strcpy(content_read, shm_area->content);

        sleep(1);

        // Get the number of readers
        sem_wait(sem_n_readers);
        int n_readers_instant = n_readers->n_readers;
        sem_post(sem_n_readers);

        // Print, in purple, the content read and the number of readers
        printf("READ: %s - Nº Readers: %d\n", content_read, n_readers_instant);

        // Decrease the number of readers
        sem_wait(sem_n_readers);
        n_readers->n_readers--;
        if (n_readers->n_readers == 0)
        {
            sem_post(sem_writer);
        }
        sem_post(sem_n_readers);

        // Exit the process
        exit(EXIT_SUCCESS);
    }
}

void createWriter()
{

    // Create a new process
    pid_t pid = fork();
    if (pid == 0)
    {
        sem_wait(sem_n_writers);
        n_writers->n_writers++;
        if (n_writers->n_writers == 1)
        {
            sem_wait(sem_reader);
        }
        sem_post(sem_n_writers);

        // Prepare the content to write

        // Get the current time into a string
        time_t currTime = time(NULL);
        struct tm *tm = localtime(&currTime);
        char time_str[100];
        strftime(time_str, sizeof(time_str), "%c", tm);

        // Join the pid and the time
        char content_to_add[200];
        sprintf(content_to_add, "%d: %s|", getpid(), time_str);

        sem_wait(sem_writer);
        // Concatenate the content_to_add to the shared memory area content
        strcat(shm_area->content, content_to_add);
        sem_post(sem_writer);

        // Get the number of writers
        int n_writers_instant = n_writers->n_writers;

        // Get the number of readers
        int n_readers_instant = n_readers->n_readers;

        // Print the number of writers and readers
        printf("############ \033[0;33mNº Writers: %d ; \e[1;32mNº Readers: %d\e[0;37m ############\n", n_writers_instant, n_readers_instant);

        sem_wait(sem_n_writers);
        n_writers->n_writers--;
        if (n_writers->n_writers == 0)
        {
            sem_post(sem_reader);
        }
        sem_post(sem_n_writers);

        // Exit the process
        exit(EXIT_SUCCESS);
    }
}

void waitAll()
{
    // Wait for all the processes to finish
    for (int i = 0; i <= N_READERS + N_WRITERS + 100; i++)
    {
        wait(NULL);
    }
}

int main()
{
    // Initialize the program
    initialiazeProgram();

    // Random call to create a reader or a writer, it can only create writers while the number of writers is less than the specified number, same for readers
    int writers = 0;
    int readers = 0;
    printf("Creation order:\n");
    while (writers + readers < N_READERS + N_WRITERS)
    {
        // Generate a random number between 0 and 1
        srand(time(NULL) * getpid() * rand() / 3 * getpid() * 987657755 * rand() * rand());
        int random = rand() % 2;
        if (random == 0 && writers < N_WRITERS)
        {
            createWriter();
            writers++;
        }
        else if (random == 1 && readers < N_READERS)
        {
            createReader();
            readers++;
        }
    }

    // Wait for all the processes to finish
    waitAll();

    // Terminate the program
    terminateProgram();

    return 0;
}