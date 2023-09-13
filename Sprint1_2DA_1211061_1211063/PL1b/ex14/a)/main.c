#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

//função que trata o alarme
void handle_alarm(int signo){
    char text[100];
    //escrever o texto no buffer
    sprintf(text, "\nYou were too slow and this program will end!\n");
    write(STDOUT_FILENO, text, strlen(text));
    exit(0);
}

int main(){

    size_t string_size = 255;
    char *str = calloc(string_size, sizeof(char));
    
    //definir o alarme
    signal(SIGALRM, handle_alarm);
    alarm(10);

    //ler a string do teclado com a função getline()
    printf("Type in a string (WARNING: you only have 10 seconds): ");
    getline(&str, &string_size, stdin);

    //tamanho da string
    printf("Size: %ld \n", strlen(str)-1); // -1 para não contar o \n adicionado automaticamente pelo getline()

    return 0;
}