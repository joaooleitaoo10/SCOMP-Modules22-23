#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main (){
    int file_descriptor[2]; //duas posições, uma para leitura e outra para escrita

    //variáveis de input
    int num;
    char str[100];

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
        
        printf("Enter an integer: ");
        scanf("%d", &num);
        printf("Enter a string: ");
        scanf("%s", str);

        //fechar o descritor não utilizado (leitura)
        close(file_descriptor[0]);

        //escrever no pipe
        write(file_descriptor[1], &num, sizeof(num));
        write(file_descriptor[1], str, strlen(str) + 1);

        //fechar o descritor de escrita
        close(file_descriptor[1]);
    }else{
        //processo filho
        read(file_descriptor[0], &num, sizeof(num));
        read(file_descriptor[0], str, sizeof(str));

        //fechar o descritor de não utilizado (escrita)
        close(file_descriptor[1]);

        //imprimir os valores lidos do pipe
        printf("\nResults\nInteger: %d\nString: %s\n", num, str);

        //fechar o descritor de leitura
        close(file_descriptor[0]);
    }

    return 0;
}