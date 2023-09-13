#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int main()
{

    // create the pipes
    int file_descriptor[2][2];

    pid_t pid;

    int credit = 20;

    if (pipe(file_descriptor[0]) == -1 || pipe(file_descriptor[1]) == -1)
    {
        perror("pipe");
        exit(1);
    }

    if ((pid = fork()) == -1)
    {
        perror("fork");
        exit(1);
    }

    if (pid == 0)
    { // processo filho
        // Fechar os descritores que não são utilizados pelo processo filho
        close(file_descriptor[0][1]);
        close(file_descriptor[1][0]);

        while(1){
            sleep(1);
        // Lê do pipe se tem créditos para apostar
        int can_I_bet;
        read(file_descriptor[0][0], &can_I_bet, sizeof(int));

        //Se não tiver créditos para apostar termina
        if (can_I_bet == 0)
        {
            close(file_descriptor[0][0]);
            close(file_descriptor[1][1]);
            exit(0);
        }

        //Gera um valor aleatório para a aposta
        srand(time(NULL) + getpid());
        int bet = rand() % 6;

        //Escreve a aposta no pipe para o pai ler
        write(file_descriptor[1][1], &bet, sizeof(int));

        //Lê do pipe o novo valor de créditos
        int new_credits;
        read(file_descriptor[0][0], &new_credits, sizeof(int));

        //Imprime o novo valor de créditos
        printf("Novo valor de créditos: %d€ \n", new_credits);
        }
        //Fecha os descritores de leitura e escrita
        close(file_descriptor[0][0]);
        close(file_descriptor[1][1]);

        //O processo filho termina
        exit(0);
    }
    else
    { // processo pai
        //Fechar os descritores que não são utilizados pelo processo pai
        close(file_descriptor[0][0]);
        close(file_descriptor[1][1]);

        while (credit >= 5)
        {
            // a) O pai gera um valor aleatório entre 1 e 5
            srand(time(NULL)+getpid());
            int bet_key = rand() % 6;

            // b) Se o filho tiver créditos suficientes, o pai escreve 1 no pipe, senão escreve 0
            if (credit < 5)
            {
                int notification = 0;
                write(file_descriptor[0][1], &notification, sizeof(int));
            }
                int notification = 1;
                write(file_descriptor[0][1], &notification, sizeof(int));

            int child_bet;
            read(file_descriptor[1][0], &child_bet, sizeof(int));

            if (bet_key == child_bet)
            {
                //d) são adicionados 10€ em caso de ganhar a aposta
                credit += 10;
                printf("\nA aposta bateu, +10€\n\n");
            }
            else
            {
                //d) são retirados 5€ em caso de perder a aposta
                credit -= 5;
                printf("\nA aposta fracassou, -5€\n\n");
            }
            write(file_descriptor[0][1], &credit, sizeof(int));
        }
        //Fechar os descritores de leitura e escrita
        close(file_descriptor[0][1]);
        close(file_descriptor[1][0]);
        //O processo pai espera pelo processo filho
        wait(NULL);
    }

    printf("\nCréditos: %d \n", credit);

    return 0;
}