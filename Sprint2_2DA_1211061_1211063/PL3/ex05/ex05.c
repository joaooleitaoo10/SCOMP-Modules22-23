#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <signal.h>

#define NR_OPERATIONS 1000000

typedef struct
{
    int number;
} IntegerS;

int main()
{

    int fd, data_size = sizeof(IntegerS);
    IntegerS *myInt;

    // Setup the signal logic

    // Signal with the info
    siginfo_t siginfo;

    sigset_t common_set;
    sigemptyset(&common_set);
    sigaddset(&common_set, SIGUSR1);
    sigaddset(&common_set, SIGUSR2);
    sigprocmask(SIG_SETMASK, &common_set, NULL);

    sigset_t parent_set;
    sigemptyset(&parent_set);
    sigaddset(&parent_set, SIGUSR1);

    sigset_t child_set;
    sigemptyset(&child_set);
    sigaddset(&child_set, SIGUSR2);

    // Create shared memory area
    fd = shm_open("/shm_space", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
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
    myInt = (IntegerS *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (myInt == MAP_FAILED)
    {
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
    // END Create shared memory area

    // Initialize the shared memory area
    myInt->number = 50;

    // Child
    int pid = fork();
    if (pid == -1)
    {
        perror("Error creating child process");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        for (int i = 0; i < NR_OPERATIONS; i++)
        {
            myInt->number--;
            kill(getppid(), SIGUSR1);
            sigwaitinfo(&child_set, &siginfo);
        }
        exit(EXIT_SUCCESS);
    }

    // Parent
    int i;
    for (i = 0; i < NR_OPERATIONS; i++)
    {
        sigwaitinfo(&parent_set, &siginfo);
        myInt->number++;
        kill(pid, SIGUSR2);
    }

    wait(NULL);

    // Print final value
    printf("Final value: %d\n", myInt->number);

    // Delete shared memory area
    if (munmap(myInt, data_size) == -1)
    {
        perror("Error un-mmapping the file");
    }
    close(fd);
    shm_unlink("/shm_space");

    return 0;
}
