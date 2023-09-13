#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>


volatile sig_atomic_t USR1_counter = 0;

void handle_USR1(int signo, siginfo_t * sinfo, void * context) {
    USR1_counter++;
    char text[100];
    sprintf(text , "SIGUSR1 signal captured: USR1_counter == %d\n" , USR1_counter);
    write(STDOUT_FILENO , text , strlen (text));
}

int main(){

    struct sigaction action;

    //reset da variável USR1_counter
    USR1_counter = 0;

    //definir o handler
    memset(&action, 0, sizeof(action));
    action.sa_sigaction = handle_USR1;
    action.sa_flags = SA_SIGINFO | SA_NODEFER;
    sigaction(SIGUSR1, &action, NULL);

    //loop infinito
    for(;;){
            printf("I'm Working! %d\n", getpid());
            sleep(1);
        }

    return 0;
}