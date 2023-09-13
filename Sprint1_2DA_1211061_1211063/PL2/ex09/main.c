#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define TOTAL_SALES 50000
#define SALES_PER_CHILD 5000
#define MIN_QUANTITY 20

typedef struct
{
   int customer_code;
   int product_code;
   int quantity;
} Sale;

int main()
{
   Sale sales[TOTAL_SALES];
   int products[TOTAL_SALES];
   int i, j, pid, status;
   int num_products = 0;
   int pipecp[2];

   // Fill sales array with random values
   for (i = 0; i < TOTAL_SALES; i++)
   {
      sales[i].customer_code = rand() % 1000;
      sales[i].product_code = rand() % 100;
      sales[i].quantity = rand() % 50;
   }

   // Create pipe
   if (pipe(pipecp) == -1)
   {
      printf("Error creating pipe\n");
      exit(EXIT_FAILURE);
   }

   // Create child processes
   for (i = 0; i < 10; i++)
   {
      pid = fork();
      if (pid == -1)
      {
         printf("Error creating child process\n");
         exit(EXIT_FAILURE);
      }
      else if (pid == 0)
      {
         // Child process
         close(pipecp[0]);
         for (j = i * SALES_PER_CHILD; j < (i + 1) * SALES_PER_CHILD; j++)
         {
            if (sales[j].quantity > MIN_QUANTITY)
            {
               // Send product code to parent process
               write(pipecp[1], &sales[j].product_code, sizeof(int));
            }
         }
         close(pipecp[1]);
         exit(EXIT_SUCCESS);
      }
   }

   // Parent process
   close(pipecp[1]);
   for (i = 0; i < TOTAL_SALES; i++)
   {
      int product_code;
      // Read product code from pipe, check if the value is already in the products array, if not add it
      read(pipecp[0], &product_code, sizeof(int));
      int found = 0;
      for (j = 0; j < num_products; j++)
      {
         if (products[j] == product_code)
         {
            found = 1;
            break;
         }
      }
      if (found == 0)
      {
         products[num_products] = product_code;
         num_products++;
      }
   }
   close(pipecp[0]);

   // Wait for child processes to finish
   for (i = 0; i < 10; i++)
   {
      wait(&status);
   }

   // Print products array
   printf("Products sold more than %d units in a single sale:\n", MIN_QUANTITY);
   for (i = 0; i < num_products; i++)
   {
      printf("Product code: \033[0;32m%d\033[0m\n", products[i]);
   }
   printf("\n");

   return 0;
}