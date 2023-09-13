#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(){
    int file_descriptor[2]; //duas posições, uma para leitura e outra para escrita

    //start by creating the structure
    struct data{
        int number;
        char string[100];
    };

    pid_t pid;

    if(pipe(file_descriptor) < 0){
        perror("Pipe Error!");
        exit(1);
    }

    if((pid = fork()) < 0){
        perror("Fork Error!");
        exit(2);
    }

    if(pid > 0){
        //processo pai
        struct data data;
        printf("Enter a number: ");
        scanf("%d", &data.number);
        printf("Enter a string: ");
        scanf("%s", data.string);

        //fechar o descritor não utilizado (leitura)
        close(file_descriptor[0]);

        //escrever no pipe
        write(file_descriptor[1], &data, sizeof(data));

        //fechar o descritor de escrita
        close(file_descriptor[1]);
    }else{
        //processo filho
        struct data data;
        close(file_descriptor[1]);

        //ler do pipe
        read(file_descriptor[0], &data, sizeof(data));

        //imprimir os valores lidos do pipe
        printf("\nResult:\nNumber: %d\nString: %s\n", data.number, data.string);

        //fechar o descritor de leitura
        close(file_descriptor[0]);
    }

    return 0;
}