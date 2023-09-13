#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(){

    // Write a program that creates a child process and establishes with it a communicating channel through a pipe. The
    //parent process should send the contents of a text file to its child through the pipe. The child should print all the
    //received data. The parent must wait for its child to end

    int file_descriptor[2]; //duas posições, uma para leitura e outra para escrita

    //variáveis de input
    char str[500];

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
        FILE *file = fopen("test.txt", "r");

        if(file == NULL){
            perror("File Error!");
            exit(3);
        }

        //fechar o descritor não utilizado (leitura)
        close(file_descriptor[0]);

        while(fgets(str, 500, file) != NULL){
            //escrever no pipe
            write(file_descriptor[1], str, strlen(str) + 1);
        }

        //fechar o descritor de escrita
        close(file_descriptor[1]);
    }else{
        //processo filho
        char str[500];

        //fechar o descritor de não utilizado (escrita)
        close(file_descriptor[1]);

        //ler do pipe
        while(read(file_descriptor[0], str, sizeof(str)) > 0){
            //imprimir os valores lidos do pipe
            printf("\nString from file: %s\n", str);
        }

        //fechar o descritor de leitura
        close(file_descriptor[0]);
    }

    return 0;
}