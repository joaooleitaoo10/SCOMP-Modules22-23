#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>


int main(){

    //arrays
    int vec1[1000], vec2[1000];
    
    //Variáveis
    int tmp, sum = 0;

    //Pipe
    int file_descriptor[2];

    //array de processos filho
    pid_t pid[5];


    //preencher os vetores com valores aleatórios
    for(int i = 0; i < 1000; i++){
        //gerar uma nova seed
        srand(time(NULL));
        vec1[i] = rand() % 200;
        vec2[i] = rand() % 200;
    }

    //criar o pipe
    if(pipe(file_descriptor) < 0){
        perror("Pipe Error!");
        exit(1);
    }

    //criar os processos filhos
    for(int i = 0; i < 5; i++){
        if((pid[i] = fork()) < 0){
            perror("Fork Error!");
            exit(2);
        }

        if(pid[i] == 0){
            //processo filho
            for(int j = i*200; j < (i+1)*200; j++){
                tmp += vec1[j] + vec2[j];
            }

            //fechar o descritor de leitura
            close(file_descriptor[0]);

            //escrever no pipe
            write(file_descriptor[1], &tmp, sizeof(int));

            //fechar o descritor de escrita
            close(file_descriptor[1]);
            exit(0);
        }
    }

    //processo pai espera pelos filhos
    for(int i = 0; i < 5; i++){
        wait(NULL);
    }

    for(int i = 0; i < 5; i++){
        //fechar o descritor de escrita
        close(file_descriptor[1]);

        //ler do pipe
        read(file_descriptor[0], &tmp, sizeof(int));

        //fechar o descritor de leitura
        close(file_descriptor[0]);

        //somar os valores lidos do pipe
        sum += tmp;
    }

    printf("Sum: %d \n", sum);

    return 0;
}