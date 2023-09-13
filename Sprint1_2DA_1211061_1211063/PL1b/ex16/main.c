#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <string.h>

volatile sig_atomic_t USR1_counter = 0;
volatile sig_atomic_t worker = 0;  //contador de workers

void handle_USR1() {
    worker++;
    USR1_counter++;
}

void handle_USR2(){
    worker++;
}

int simulate1() {       

    //gera uma seed nova
    srand(time(NULL));

    //gera um valor aleatório entre 0 e 100
    int randomValue = rand() % 101;

    //chance de 1% de retornar 1
    if(randomValue == 1){
        return 1;
    }
    
    return 0;
}

int simulate2() {

    //gera uma seed nova
    srand(time(NULL));

    //gera um valor aleatório entre 0 e 10
    int randomValue = rand() % 11;

    if(randomValue == 1){
        return 1;
    }

    return 0;
}

void redefine_USR1(){
    simulate2();
}


int main() {

    struct sigaction action_usr1;
    struct sigaction action_usr2;
    memset(&action_usr1, 0, sizeof(action_usr1));
    memset(&action_usr2, 0, sizeof(action_usr2));
    action_usr1.sa_sigaction = handle_USR1;
    action_usr2.sa_sigaction = handle_USR2;
    action_usr1.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT;
    action_usr2.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT;
    sigaction(SIGUSR1, &action_usr1, NULL);
    sigaction(SIGUSR2, &action_usr2, NULL);

    pid_t workerProcesses[50]; 
    // O master process cria 50 worker processes
    for(int i = 0; i < 50; i++){
        if((workerProcesses[i] = fork()) == 0){
            if(simulate1() == 1){
                kill(getppid(), SIGUSR1);
            }else if(simulate1() == 0){
                kill(getppid(), SIGUSR2);
            }

            //redefinir o sinal SIGUSR1 para chamar a função simulate2()
            struct sigaction action;
            memset(&action, 0, sizeof(action));
            action.sa_sigaction = redefine_USR1;
            action.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT;
            sigaction(SIGUSR1, &action, NULL);

            //o worker process espera por um sinal do pai antes de executar a função simulate2()
            pause();
            exit(i);
        }
    }

    // O master process vai esperar por 25 sinais recebidos dos worker processes
    while(worker < 25){
       pause();
    }

    sleep(1);
    // Se receber pelo menos 1 sinal SIGUSR1, o master process envia um sinal SIGUSR1 para todos os worker processes
    if(USR1_counter < 1){
        printf("Inefficient algorithm \n");
        for(int i = 0; i < 50; i++){
            kill(workerProcesses[i], SIGINT);
        }
    }else if(USR1_counter >= 1){
        printf("Amazing Algorithm\n");
        for(int i = 0; i < 50; i++){
            kill(workerProcesses[i], SIGUSR1);
        }
    }

    // O master process espera que todos os worker processes terminem
    for(int i = 0; i < 50; i++){
        wait(NULL);
    }        

    return 0;
}