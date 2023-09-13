#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

int main (void){
    int randomIntegers[2000];
    int n, iterator = 0, status, index;

    pid_t pids[10];

    //generate a new seed
    srand(time(NULL));


    //Fill the array with random numbers between 0 and 1000
    for(int i = 0; i<2000; i++){
        randomIntegers[i] = rand() % 50;
    }

    //Give n a random number between 0 and 100
    n = rand() % 50;

/* The following code is for testing purposes
    int test = 200;
    for(int i = 0; i<2000; i++){
        if(i == test){
            test +=200;
            printf("\n\n");
        }
        printf("%d ", randomIntegers[i]);
    }


    printf("\n\n");*/

    

    for(int i = 0; i < 10; i++){
        //Create a child process
        if((pids[i] = fork()) == 0){
            for(int j = (0 + iterator); j < (200 + iterator); j++){
                if(randomIntegers[j] == n){
                    index = j - (200 * i);
                    exit(index);
                }
            }           
            exit(255);
        }
        iterator += 200;
    }

    for(int i = 0; i < 10; i++){
        waitpid(pids[i], &status, 0);
            if(WEXITSTATUS(status) != 255){
                printf("The number %d was found at index %d\n", n, WEXITSTATUS(status));
            }
        
    }

}