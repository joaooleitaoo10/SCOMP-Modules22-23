#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

typedef struct{
    int first_value;
    int second_value;
} values;

int main(){
    // Criar o apotandor para a estrutura "values".  
    values *ptr;

    // Criação do bloco de memória para escrita.
    int data_size = sizeof(values);
    int fd = shm_open("/shmtest", O_CREAT | O_EXCL | O_RDWR,S_IRUSR | S_IWUSR);
    ftruncate(fd, data_size);
    ptr = (values*)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Colocar os valores 10000 no primeiro valor e 500 no segundo valor.
    ptr->first_value = 10000;
    ptr->second_value = 500;

    // Criação do processo filho.
    pid_t pid;

    if((pid = fork()) < 0){
        perror("Fork Error!");
        exit(1);
    }

    // Processo filho.
    if(pid == 0){
        for(int iterator = 0; iterator < 1000000; iterator++){
            ptr->first_value++;
            ptr->second_value--;
        }
        exit(0);
    } else { // Processo pai.
        for(int iterator = 0; iterator < 1000000; iterator++){
            ptr->first_value--;
            ptr->second_value++;
        }
    }

    // Imprimir os valores lidos.
    printf("First value: %d \t Second value: %d \n", ptr->first_value, ptr->second_value);
     
    shm_unlink("/shmtest");
    return 0;
}