#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "spawn_childs.h"

int main(void){
    int children = spawn_childs(6), status;

    if(children == 0){ //parent process
        for(int i = 0; i < 6; i++){
            wait(&status); //ensurance that the parent process waits for all the child processes to finish
        }
        printf("Parent| Index |\n");
        //print the parent ensuring that the index is 0
        printf("%d  |   %d   |\n", getpid(), 0);
    }else{
        printf("Child | Index | (Index x 2)\n");
        //print the child processes by order of creation
        printf("%d  |   %d   |     %d\n", getpid(), children, children * 2);
    }
    return 0;
}