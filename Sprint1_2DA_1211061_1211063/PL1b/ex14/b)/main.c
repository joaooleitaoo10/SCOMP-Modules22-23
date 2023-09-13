#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

void handle_alarm(int signo){
    char text[100];
    //escrever o texto no buffer
    sprintf(text, "You were too slow and this program will end!\n");
    write(STDOUT_FILENO, text, strlen(text));
    exit(0);
}

int main(){
    pid_t pid = fork();
    size_t string_size = 255;
    char *str = calloc(string_size, sizeof(char));


    if (pid == 0){
        //processo filho       
        //ler a string do teclado com a função getline()
        printf("Type in a string (WARNING: you only have 10 seconds): ");
        getline(&str, &string_size, stdin);
        printf("\nSize: %ld \n", strlen(str)-1); // -1 para não contar o \n adicionado automaticamente pelo getline()
        exit(0);
    }else{
        //processo pai
        signal(SIGALRM, handle_alarm);
        alarm(10);
        wait(NULL);
    }

    return 0;
}