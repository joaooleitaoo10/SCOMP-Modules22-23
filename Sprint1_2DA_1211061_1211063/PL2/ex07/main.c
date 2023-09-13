#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define ARRAY_SIZE 1000
#define N_CHILDREN 5
#define CHUNK_SIZE ARRAY_SIZE / N_CHILDREN

int main()
{
   int vec1[ARRAY_SIZE], vec2[ARRAY_SIZE], result[ARRAY_SIZE];
   int i, j;
   // Initialize vectors with some values
   for (i = 0; i < ARRAY_SIZE; i++)
   {
      vec1[i] = i;
      vec2[i] = i + 1;
   }
   // Create the pipes and assure that they were created successfully
   int pipes[N_CHILDREN][2];
   for (i = 0; i < N_CHILDREN; i++)
   {
      if (pipe(pipes[i]) == -1)
      {
         exit(EXIT_FAILURE);
      }
   }

   // Create all the children to perform the sum
   pid_t pids[N_CHILDREN];
   for (i = 0; i < N_CHILDREN; i++)
   {
      // Create child process and assure that it was created successfully
      pids[i] = fork();
      if (pids[i] == -1)
      {
         exit(EXIT_FAILURE);
      }
      else if (pids[i] == 0)
      {
         // Child process that performs the sum

         // Close read end of pipe
         close(pipes[i][0]);

         /////////// WRONG, deve somar todos, 1 a 1 e ir mandando para o daddy e depois o dady guarda, na ordem certa (usar os waits corretos) os resultados das somas //////////////////

         // Sum each position of the vectors and write the result to the pipe
         for (j = i * CHUNK_SIZE; j < (i + 1) * CHUNK_SIZE; j++)
         {
            int sum = vec1[j] + vec2[j];
            if (write(pipes[i][1], &sum, sizeof(sum)) == -1)
            {
               exit(EXIT_FAILURE);
            }
         }

         close(pipes[i][1]);
         exit(EXIT_SUCCESS);
      }
   }

   // Parent process
   int status;
   pid_t wpid;
   int sum;
   // wait for all children to finish in order and store the result in the result array ordered
   for (i = 0; i < N_CHILDREN; i++)
   {
      // Close write end of pipe
      close(pipes[i][1]);

      // Wait for the child to finish and assure that it finished successfully
      wpid = waitpid(pids[i], &status, 0);

      if (wpid == -1)
      {
         exit(EXIT_FAILURE);
      }
      else if (WIFEXITED(status))
      {
         // Read the result from the pipe
         for (j = i * CHUNK_SIZE; j < (i + 1) * CHUNK_SIZE; j++)
         {
            if (read(pipes[i][0], &sum, sizeof(sum)) == -1)
            {
               exit(EXIT_FAILURE);
            }
            result[j] = sum;
         }
      }
      else
      {
         exit(EXIT_FAILURE);
      }
   }

   // Print result array
   printf("Result array:\n");
   for (i = 0; i < ARRAY_SIZE; i++)
   {
      printf("%d - %d\n", i, result[i]);
   }
   printf("\n");

   return EXIT_SUCCESS;
}