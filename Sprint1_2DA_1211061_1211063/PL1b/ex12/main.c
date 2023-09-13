#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

volatile sig_atomic_t sigusr1_count = 0;

void handle_USR1(int sign, siginfo_t *info, void *context){
    sigusr1_count++;
}

int main(){

    struct sigaction action;

    //define o handler
    memset(&action, 0, sizeof(action));
    action.sa_sigaction = handle_USR1;
    action.sa_flags = SA_SIGINFO | SA_NOCLDWAIT | SA_NOCLDSTOP;
    sigaction(SIGUSR1, &action, NULL);

    //geração de 5 processos filhos que imprimem os números inteiros no intervalo [i × 200, (i + 5) × 200[
    for(int i = 0; i < 5; i++){
        if(fork() == 0){
            for(int j = i * 200; j < (i + 5) * 200; j++){
                printf("%d ", j);
            }
            printf("\n\n");
            //notificar o processo pai com o sinal SIGUSR1
            kill(getppid(), SIGUSR1);
            exit(0);
        }
    }   
    
    //processo pai entra no loop a executar a função pause() até que não hajam mais processos filhos a executar
    while(sigusr1_count < 5){
        pause();
    }

    //processo pai espera que todos os processos filhos terminem
    for(int i = 0; i < 5; i++){
        wait(NULL);
    }

    return 0;
}