#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

typedef struct {
    char file_path[100];
    char word[50];
    int number_of_occurences;
} found_words;

int main(){
    // Definir as variáveis
    pid_t pid;

    // Criar a memória partilhada
    int data_size = 10 * sizeof(found_words); // Multiplica-se por 10 para ser o tamanho para os 10 filhos
    int fd = shm_open("/shmtest", O_CREAT | O_EXCL | O_RDWR,S_IRUSR | S_IWUSR);
    ftruncate(fd, data_size);
    found_words *data = (found_words*) mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
     
    // Preencher a estrutura com informação dos ficheiros
    strcpy(data[0].file_path, "child1.txt");
    strcpy(data[0].word, "I'm");
    strcpy(data[1].file_path, "child2.txt");
    strcpy(data[1].word, "the");
    strcpy(data[2].file_path, "child3.txt");
    strcpy(data[2].word, "Master");
    strcpy(data[3].file_path, "child4.txt");
    strcpy(data[3].word, "of");
    strcpy(data[4].file_path, "child5.txt");
    strcpy(data[4].word, "Puppets");
    strcpy(data[5].file_path, "child6.txt");
    strcpy(data[5].word, "and");
    strcpy(data[6].file_path, "child7.txt");
    strcpy(data[6].word, "I'm");
    strcpy(data[7].file_path, "child8.txt");
    strcpy(data[7].word, "Pulling");
    strcpy(data[8].file_path, "child9.txt");
    strcpy(data[8].word, "Your");
    strcpy(data[9].file_path, "child10.txt");
    strcpy(data[9].word, "Strings");



    // Criar os filhos
    for(int i = 0; i<10; i++){
        if((pid = fork()) == -1){
            perror("fork error!");
            exit(EXIT_FAILURE);
        }else if(pid == 0){ // Filho
            // Abrir ficheiro
            FILE *fp;
            char file_text[50];
            fp = fopen(data[i].file_path, "r");
            int count = 0;

            // Verificar se o ficheiro está válido.
            if((fp = fopen(data[i].file_path, "r")) == NULL){
                perror("Please, provide a valid file!");
                exit(EXIT_FAILURE);
            }

            // Enquanto não atingir o EOF (End of File), vai ser procurada a palavra em questão.
            while(fscanf(fp, "%s", file_text) != EOF){
                if(strcmp(file_text, data[i].word) == 0){
                    count++; // Contador para auxiliar a contagem de aparências da palavra.
                }
            }

            fclose(fp);  // Fechar o ficheiro.
            data[i].number_of_occurences = count; // Guardar o número de aparências no filho.
            close(fd);  // Fechar a Shared Memory
            exit(EXIT_SUCCESS); // Terminar o processo filho.
        }
    }

    // O processo pai espera por todos os filhos.
    for(int i = 0; i<10; i++){
        wait(NULL);
    }

    // Imprimir o número de aparências de cada palavra em cada ficheiro.
    for(int i = 0; i<10; i++){
        printf("The word '%s' appears %d times in the child %d file.\n\n", data[i].word, data[i].number_of_occurences, i+1);
        sleep(1);
    }

    // Fechar a memória partilhada
    munmap(data, data_size);
    close(fd);
    shm_unlink("/shmtest");
    
    return 0;
}