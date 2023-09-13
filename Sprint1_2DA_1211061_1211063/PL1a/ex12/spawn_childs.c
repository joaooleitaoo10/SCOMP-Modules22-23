#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int spawn_childs(int n){

    pid_t pid;

    //ensurance that the the child processes are created with indexes starting in 1
    for(int index = 1; index < n; index++){ 
        pid = fork();
        if(pid == 0){
            //return index of the child
            return index;
        }
    }
    //return 0 to the parent process
    return 0;
}