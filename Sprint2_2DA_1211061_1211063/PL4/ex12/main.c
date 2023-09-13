#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <semaphore.h>

#include "up_down.h"

#define NR_INTS 10
#define NR_EXCHANGED 30
#define NR_PRODUCERS 2

typedef struct
{
   int numbers[NR_INTS];
   int writerIdx;
   int readerIdx;
} Circular_buffer;

int main()
{
   // Criação da memória partilhada
   int data_size = sizeof(Circular_buffer);
   int fd = shm_open("/shm_test", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
   ftruncate(fd, data_size);
   Circular_buffer *myBuffer = (Circular_buffer *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

   // Criação dos semáforos relativos ao buffer circular
   sem_t *mutex;
   sem_t *full;
   sem_t *empty;

   if ((mutex = sem_open("/sem_ex12_mutex", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
   {
      perror("Error at sem_open().\n");
      exit(EXIT_FAILURE);
   }

   if ((full = sem_open("/sem_ex12_full", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED)
   {
      perror("Error at sem_open().\n");
      exit(EXIT_FAILURE);
   }

   if ((empty = sem_open("/sem_ex12_empty", O_CREAT | O_EXCL, 0644, NR_INTS)) == SEM_FAILED)
   {
      perror("Error at sem_open().\n");
      exit(EXIT_FAILURE);
   }

   // Inicializações de variáveis
   myBuffer->readerIdx = 0;
   myBuffer->writerIdx = 0;
   pid_t pid;

   // Criação dos processos filhos (produtores)
   for (int i = 0; i < NR_PRODUCERS; i++)
   {
      if ((pid = fork()) == -1)
      {
         perror("Error creating child process");
         exit(EXIT_FAILURE);
      }
      else if (pid == 0)
      {
         // Processos filho (produtores)
         srand(time(NULL) + getpid()); // gerar uma nova semente

         for (int j = i; j < NR_EXCHANGED; j+=NR_PRODUCERS) // Incremento a 2 para que os 2 produtores escrevam em posições diferentes
         {
            // Esperar que o buffer esteja vazio
            down(empty);
            down(mutex);

            // Escrever no buffer
            myBuffer->numbers[myBuffer->writerIdx] = j;
            printf("\e[0;35mProducer wrote: %d\n", myBuffer->numbers[myBuffer->writerIdx]);

            // Atualizar o índice do escritor
            myBuffer->writerIdx = (myBuffer->writerIdx + 1) % NR_INTS;

            // Liberta o buffer e sinaliza que está cheio
            up(mutex);
            up(full);

            sleep(1); // Espera 1 segundo (requisito)
         }
         exit(EXIT_SUCCESS);
      }
   }

   // Processo pai (consumidor)
   for (int i = 0; i < NR_EXCHANGED; i++)
   {
      // Espera que o buffer esteja cheio
      down(full);
      down(mutex);

      // Lê do buffer
      printf("\e[1;32mConsumer read: %d\n", myBuffer->numbers[myBuffer->readerIdx]);

      // Atualiza o indice do leitor
      myBuffer->readerIdx = (myBuffer->readerIdx + 1) % NR_INTS;

      // Liberta o buffer e sinaliza que está vazio
      up(mutex);
      up(empty);

      sleep(2); // Espera 2 segundos (requisito)
   }

   // Pai espera pelos filhos
   wait(NULL);

   // Fechar a memória partilhada
   munmap(myBuffer, data_size);
   close(fd);
   shm_unlink("/shm_test");

   // Fecha os semáforos
   sem_close(mutex);
   sem_unlink("/sem_ex12_mutex");
   sem_close(full);
   sem_unlink("/sem_ex12_full");
   sem_close(empty);
   sem_unlink("/sem_ex12_empty");

   return 0;
}
