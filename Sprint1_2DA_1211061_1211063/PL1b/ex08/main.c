#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>



void handle_USR1(int signo, siginfo_t *info, void *context) {
    char text [80];
    sprintf(text , "I captured a SIGUSR1 sent by the process with PID %d\n" , info->si_pid);
    write(STDOUT_FILENO , text , strlen (text));
}


int main(){

struct sigaction act;

memset(&act, 0, sizeof(struct sigaction));

sigemptyset(&act.sa_mask); /* No signals blocked */
act.sa_sigaction = handle_USR1;
act.sa_flags = SA_SIGINFO;
sigaction(SIGUSR1, &act, NULL);

for (; ;) {
    sleep(1);
    kill(getpid(), SIGUSR1);
}

return 0;
}