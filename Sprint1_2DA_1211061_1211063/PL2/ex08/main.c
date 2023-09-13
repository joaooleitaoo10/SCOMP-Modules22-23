#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

typedef struct {
    char message[4];
    int round_number;
} Win_the_pipe;

int main() {

    // Criação do pipe partilhado por todos os processos
    int file_descriptor[2];

    // Array de processos filho
    pid_t pid[10];

    // Struct para guardar a mensagem e o número do round
    Win_the_pipe game;

    // Variável para guardar o round do processo filho vencedor
    int status;

    if (pipe(file_descriptor) == -1) {
        perror("pipe");
        exit(1);
    }

    for (int i = 0; i < 10; i++) {

        if ((pid[i] = fork()) == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid[i] == 0) { // Processo filho
            // Fechar o descritor de escrita
            close(file_descriptor[1]);
            // Enquanto houver dados para ler do pipe
            while (read(file_descriptor[0], &game, sizeof(game)) > 0) {
                // Se a mensagem for "Win" através da função strcmp que compara duas strings caracter a caracter
                if (strcmp(game.message, "Win") == 0) {
                    printf("%s round %d\n",game.message, game.round_number);
                    // Termina o processo filho e retorna a round
                    exit(game.round_number);
                }
            }
            // Fechar o descritor de leitura
            close(file_descriptor[0]);
        }
    }

    // Fechar o descritor de leitura
    close(file_descriptor[0]);

    for (int i = 0; i < 10; i++) {
        sleep(2);

        // Atribuir o número do round à struct
        game.round_number = i+1;

        // Copiar a string "Win" para a struct através da função strcpy
        strcpy(game.message, "Win");

        // Escrever no pipe
        write(file_descriptor[1], &game, sizeof(game));
    }

    // Fechar o descritor de escrita
    close(file_descriptor[1]);

    // Processo pai espera que os processos filho terminem
    for (int i = 0; i < 10; i++) {
        waitpid(pid[i], &status, 0);
        printf("Child %d won round %d\n", pid[i], WEXITSTATUS(status));
    }

    return 0;
}