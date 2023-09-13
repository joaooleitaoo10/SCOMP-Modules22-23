#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/time.h>

typedef struct {
    int numbers[10];
    int canread;
    int canwrite;
} Number;

int main(){

    int fd = shm_open("/ex04", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(fd, sizeof(Number));

    Number *number = (Number *) mmap(NULL, sizeof(Number), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    for(int i=0;i<5;i++){
        printf("Numbers of Writer:\n");
        number->canread = 0; // Bloqueia o leitor
        for(int i=0;i<10;i++){
            srand((unsigned int) time(NULL) + i);
            number->numbers[i] =  rand() % 20;
            printf("%d : %d\n", i, number->numbers[i]);
        }
        printf("\n");
        number->canread = 1; // Desbloqueia o leitor
        number->canwrite = 0; // Bloqueia o escritor
     
        while (number->canwrite == 0){ // Espera até que o leitor leia
            sleep(1);
        }
    }

    munmap(number, sizeof(Number));
    return 0;
}
