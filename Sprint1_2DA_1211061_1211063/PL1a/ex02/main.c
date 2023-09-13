#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define SIZE 3

int main(void){

    char *prints[3] = {"I'm..", "the..", "father!"};

    pid_t pid;

    for(int i = 0; i < SIZE; i++){
        pid = fork();
        if(pid == 0){
            printf("I'll never join you\n");
            exit(0);
        }else{
            printf("%s\n", prints[i]);
        }
    }

    return 0;
}