#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define LOCAL_MAX 100

typedef struct{
    int array[LOCAL_MAX];
} values;

int main(){
    // Inicialização das variáveis essenciais
    pid_t pid;
    int maxEach = 0, maxGlobal = 0, iterator = 0; 

    // Geração de uma nova seed
    srand(time(NULL));
 
    // Inicialização do array de 1000 inteiros aleatórios
    int thousandIntegers[1000];
    for(int j = 0; j < 1000; j++){
        thousandIntegers[j] = rand() % 1001;
    }    

    // Criação do bloco de memória partilhada
    int data_size = sizeof(values);
    int fd = shm_open("/shmtest", O_CREAT | O_EXCL | O_RDWR,S_IRUSR | S_IWUSR);
    ftruncate(fd, data_size);
    values *ptr = (values*)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        
    // Criação de 10 processos filhos    
    for(int i = 0; i < 10; i++){
        if((pid = fork()) == -1){
            perror("Fork Error!");
            exit(EXIT_FAILURE);
        }
                
        maxEach = 0; // Reiniciar o valor da variável de controlo de máximos locais

        if(pid == 0){ // Processo filho
            for(int i = (0 + iterator); i < (100 + iterator); i++){
                if(thousandIntegers[i] > maxEach){
                    maxEach = thousandIntegers[i];
                }
            }
            ptr->array[i] = maxEach; // Guardar o máximo local no array partilhado
            exit(0);
        }
        iterator += 100; // Incrementar o iterador para o próximo bloco de 100 inteiros
    }

    // Esperar que os 10 processos filhos terminem
    for(int j = 0; j < 10; j++){
            wait(NULL);
    }

    for(int i = 0; i < 10; i++){
        if(ptr->array[i] > maxGlobal){
                maxGlobal = ptr->array[i];
        }
    }

    for(int i = 0; i < 10; i++){
        printf("Array[%d] = %d\n", i, ptr->array[i]);
    }
    
    printf("Max Global: %d\n" , maxGlobal);

    // Processo pai elimina o bloco de memória partilhada
    munmap(ptr, data_size);
    close(fd);
    shm_unlink("/shmtest");

    return 0;
}