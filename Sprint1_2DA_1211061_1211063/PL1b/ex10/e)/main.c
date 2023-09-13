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
    sleep(1);
}

int main(){

    struct sigaction action;

    USR1_counter = 0;

    //define o handler
    memset(&action, 0, sizeof(action));
    //coloca todos os sinais na máscara
    sigemptyset(&action.sa_mask);
    
    action.sa_sigaction = handle_USR1;
    action.sa_flags = SA_SIGINFO | SA_NODEFER;
    sigaction(SIGUSR1, &action, NULL);

    //cria o processo filho
    pid_t pid = fork();

    //preparar o uso da função nanosleep()
    struct timespec sleep_time;
    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = 10000000;

    if(pid == 0){
        //filho passa 12 sinais USR1 para o pai e espera 10ms entre cada
        for(int i = 0; i < 12; i++){
            kill(getppid(), SIGUSR1);
            nanosleep(&sleep_time, NULL);
        }
        kill(getppid(), SIGINT);
    } else if (pid > 0){
        //loop infinito
        for(;;){
            printf("I'm Working! %d\n", getpid());
            sleep(1);
        }
    }

    return 0;
}