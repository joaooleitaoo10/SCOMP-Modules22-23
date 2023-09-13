#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/wait.h> /* waitpid() */
#include <unistd.h> /* fork() */
#include <sys/types.h> /* getpid(), getppid() */

int main() {
    pid_t p;
    if (fork() == 0) {
        printf("PID = %d\n", getpid());
        exit(0);
    }
    if ((p = fork()) == 0) {
        printf("PID = %d\n", getpid());
        exit(0);
    }
    printf("Parent PID = %d\n", getpid());
    printf("Waiting... (for PID = %d)\n", p);

    waitpid(p, NULL, 0);
    printf("Enter loop\n");
    while (1);
    return 0;

}