#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

typedef struct {
    int numbers[10];
    int canread;
    int canwrite;
} Number;


int main(){

    int fd=shm_open("/ex04", O_CREAT|O_RDWR, S_IRUSR | S_IWUSR);
    
    Number *number= (Number *)mmap(NULL, sizeof(Number), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    for(int i=0;i<5;i++){ // Sequência de 5 leituras
        while (number->canread == 0); // Espera até que o escritor escreva
        number->canwrite = 0; // Bloqueia o escritor
        int sum=0;
        printf("Numbers of Reader:\n");
        for(int i=0;i<10;i++){
            sum+=number->numbers[i];
            printf("%d : %d\n", i, number->numbers[i]);
        }
        printf("Average: %d\n\n", sum/10);
        number->canwrite = 1; // Desbloqueia o escritor
        sleep(2); // Espera 2 segundos
    }
    return 0;
}