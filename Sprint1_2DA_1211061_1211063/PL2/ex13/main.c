#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define TOTAL_PIECES 1000
#define PIECES_CUT_PER_EXECUTION 5
#define PIECES_FOLD_PER_EXECUTION 5
#define PIECES_WELD_PER_EXECUTION 10
#define BATCHES_PER_TRANSFER 2
#define PIECES_PER_PACK 100

int main()
{
   // Instantiate variables
   int pipes[4][2];
   pid_t pids[4];

   // Create pipes for communication between machines in a for loop and assure that they are created successfully
   for (int i = 0; i < 4; i++)
   {
      if (pipe(pipes[i]) == -1)
      {
         printf("Pipe creation failed!\n");
         exit(EXIT_FAILURE);
      }
   }

   // M1 - Machine that cuts the pieces
   pids[0] = fork();
   if (pids[0] == 0)
   { // child process

      close(pipes[0][0]); // close read end of pipe used to receive data from M1
      int total_pieces_cut = 0;
      int pieces_per_exec = PIECES_CUT_PER_EXECUTION;
      for (int i = 0; i < TOTAL_PIECES; i += PIECES_CUT_PER_EXECUTION)
      {
         // Print the number of pieces cut
         printf("\033[0;31mM1: cutted %d pieces\033[0m \n", PIECES_CUT_PER_EXECUTION);

         // print the number of pieces cut in total
         total_pieces_cut += PIECES_CUT_PER_EXECUTION;
         printf("\033[0;31mM1: TOTAL cutted %d pieces in total\033[0m \n", total_pieces_cut);

         // Sending the cut pieces to M2
         write(pipes[0][1], &pieces_per_exec, sizeof(int));
      }
      close(pipes[0][1]); // close write end of pipe used to send data to M2
      exit(EXIT_SUCCESS);
   }

   // M2 - Machine that folds the pieces
   pids[1] = fork();
   if (pids[1] == 0)
   { // child process

      close(pipes[0][1]); // close write end of pipe used to send data to M2
      close(pipes[1][0]); // close read end of pipe used to receive data from M3
      int total_pieces_folded = 0;
      // Fold the pieces and send them to M3, until all pieces are folded
      for (int i = 0; i < TOTAL_PIECES; i += PIECES_FOLD_PER_EXECUTION)
      {
         // Read the pieces from M1
         int pieces;
         read(pipes[0][0], &pieces, sizeof(int));
         printf("\033[0;33mM2: received %d pieces from M1\033[0m \n", pieces);

         // Print the number of pieces folded
         printf("\033[0;33mM2: folded %d pieces\033[0m \n", pieces);

         // print the number of pieces folded in total
         total_pieces_folded += pieces;
         printf("\033[0;33mM2: TOTAL folded %d pieces in total\033[0m \n", total_pieces_folded);

         // Sending the folded pieces to M3
         write(pipes[1][1], &pieces, sizeof(int));
      }

      close(pipes[0][0]); // close read end of pipe used to receive data from M1
      close(pipes[1][1]); // close write end of pipe used to send data to M3
      exit(EXIT_SUCCESS);
   }

   // M3 - Machine that welds the pieces
   pids[2] = fork();
   if (pids[2] == 0)
   { // child process

      close(pipes[1][1]); // close write end of pipe used to send data to M3
      close(pipes[2][0]); // close read end of pipe used to receive data from M4

      int total_pieces_welded = 0;
      // Weld the pieces and send them to M4, until all pieces are welded
      for (int i = 0; i < TOTAL_PIECES; i += PIECES_WELD_PER_EXECUTION)
      {
         // Read the pieces from M2
         int pieces_to_weld = 0, pieces_recieved;
         while (pieces_to_weld < PIECES_WELD_PER_EXECUTION)
         {
            read(pipes[1][0], &pieces_recieved, sizeof(int));
            printf("\033[0;35mM3: received %d pieces from M2\033[0m \n", pieces_recieved);
            pieces_to_weld += pieces_recieved;
         }

         // Print the number of pieces welded
         printf("\033[0;35mM3: welded %d pieces\033[0m \n", pieces_to_weld);

         // print the number of pieces welded in total
         total_pieces_welded += pieces_to_weld;
         printf("\033[0;35mM3: TOTAL welded %d pieces in total\033[0m \n", total_pieces_welded);

         // Sending the welded pieces to M4
         write(pipes[2][1], &pieces_to_weld, sizeof(int));
      }

      close(pipes[1][0]); // close read end of pipe used to receive data from M2
      close(pipes[2][1]); // close write end of pipe used to send data to M4
      exit(EXIT_SUCCESS);
   }

   // M4 - Machine that packs the pieces
   pids[3] = fork();
   if (pids[3] == 0)
   {
      int total_pieces_packed = 0;
      // Pack the pieces and send them to A1, until all pieces are packed
      for (int i = 0; i < TOTAL_PIECES; i += PIECES_PER_PACK)
      {
         // Read the pieces from M3
         int pieces_recieved, pieces_to_pack = 0;

         while (pieces_to_pack < PIECES_PER_PACK)
         {
            read(pipes[2][0], &pieces_recieved, sizeof(int));
            printf("\033[0;32mM4: received %d pieces from M3\033[0m \n", pieces_recieved);
            pieces_to_pack += pieces_recieved;
         }

         // Print the number of pieces packed
         printf("\033[0;32mM4: packed %d pieces\033[0m \n", pieces_to_pack);

         // print the number of pieces packed in total
         total_pieces_packed += pieces_to_pack;
         printf("\033[0;32mM4: TOTAL packed %d pieces in total\033[0m \n", total_pieces_packed);

         // Sending the packed pieces to A1
         write(pipes[3][1], &pieces_to_pack, sizeof(int));
      }

      close(pipes[2][0]); // close read end of pipe used to receive data from M3
      close(pipes[3][1]); // close write end of pipe used to send data to A1
      exit(EXIT_SUCCESS);
   }

   // A1 - Storage area
   int total_inventory = 0;
   // Receive the packed pieces from M4, until all pieces are received
   while (total_inventory < TOTAL_PIECES)
   {
      // read packed pieces from pipe
      int inventory_update;
      read(pipes[3][0], &inventory_update, sizeof(int));
      printf("\033[0;36mA1: received %d pieces from M4\033[0m \n", inventory_update);

      total_inventory += inventory_update;
      // display current inventory
      printf("\033[0;36mA1: Current inventory: %d\033[0m \n", total_inventory);
   }

   close(pipes[3][0]); // close read end of pipe used to receive data from M4

   // Wait for all machines to finish and assure that they are finished successfully
   int status;
   if (waitpid(pids[0], &status, 0) != -1)
   {
      printf("M1 finished successfully!\n");
   }
   if (waitpid(pids[1], &status, 0) != -1)
   {
      printf("M2 finished successfully!\n");
   }
   if (waitpid(pids[2], &status, 0) != -1)
   {
      printf("M3 finished successfully!\n");
   }
   if (waitpid(pids[3], &status, 0) != -1)
   {
      printf("M4 finished successfully!\n");
   }

   return 0;
}