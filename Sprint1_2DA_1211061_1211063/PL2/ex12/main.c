#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

// Struct dos produtos
struct Product {
    int barcode;
    char * product_name;
    int price;
};

int main() {
    // Criação de 6 pipes
    int file_descriptor[6][2];

    // Verificar se os Pipes foram criados com sucesso
    for (int i = 0; i < 6; i++) {
        if (pipe(file_descriptor[i]) < 0) {
            perror("Pipe Error!\n");
            exit(1);
        }
    }

    // Array de 5 processos filhos
    pid_t pid[5];

    for (int i = 0; i < 5; i++) {
        //Verificar se os processos filho foram devidamente criados
        if ((pid[i] = fork()) < 0) {
            perror("Fork Error!\n");
            exit(EXIT_FAILURE);
        }

        // Processo filho (quem vai ler os códigos de barras)
        if (pid[i] == 0) {
            // Fechar os descritores que não vão ser usados
            close(file_descriptor[5][0]);
            for (int i_pipe = 0; i_pipe < 5; i_pipe++) {
                close(file_descriptor[i_pipe][1]);
            }

            //Variáveis correspondentes às especificações do produto (na struct)
            int barcode;
            char * product_name;
            int price;

            // Geração de uma nova seed
            srand(time(NULL) + getpid());

            // Geração de um valor aleatório entre 0-5 para o código de barras, por questões de simplicidade
            barcode = rand() % 5;

            // Escrever o código de barras no pipe
            write(file_descriptor[5][1], &barcode, sizeof(int));

            // Ler do pipe as especificações do produto
            read(file_descriptor[i][0], &product_name, sizeof(char *));
            read(file_descriptor[i][0], &price, sizeof(int));

            // Imprimir as especificações do produto correspondente ao código de barras pedido
            printf("Código de Barras: %d | Nome: %s | Preço: %d€\n", barcode, product_name, price);

            // exit
            exit(i);
        }
    }

    // Processo Pai (quem vai fornecer a informação dos produtos para um dado codigo de barras)
    
    // Base de dados dos produtos
    struct Product products[5] = {
        {1, "Banana", 10},
        {2, "Arroz", 1},
        {3, "Massa", 3},
        {4, "Água", 2},
        {5, "Chouriço", 1},
    };

    // Fechar o descritor que não vai ser usado
    close(file_descriptor[5][1]);

    for (int i = 0; i < 5; i++) {
        int barcode;
    
        // Ler do pipe o código de barras
        read(file_descriptor[5][0], &barcode, sizeof(int));

        // Passar por via struct as características do produto correspondente ao código de barras
        struct Product product = products[barcode];

        // Escrever no pipe as características do produto
        write(file_descriptor[i][1], &product.product_name, sizeof(char *));
        write(file_descriptor[i][1], &product.price, sizeof(int));

        // Fechar o descritor de escrita
        close(file_descriptor[i][1]);
    }

    // Fechar o descritor de leitura
    close(file_descriptor[5][0]);

    // O processo pai espera pelos filhos
    for (int i = 0; i < 5; i++) {
        wait(NULL);
    }

    return 0;
}