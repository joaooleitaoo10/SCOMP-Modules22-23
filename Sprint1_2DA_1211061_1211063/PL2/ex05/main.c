#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define MAX_MSG_SIZE 256

int main()
{
   int up_pipe[2], down_pipe[2];
   pid_t pid;

   // Create the pipes and assure they were created successfully
   if (pipe(up_pipe) == -1 || pipe(down_pipe) == -1)
   {
      exit(EXIT_FAILURE);
   }

   // Create the child process (client) and assure it was created successfully
   pid = fork();
   if (pid == -1)
   {
      exit(EXIT_FAILURE);
   }

   if (pid == 0)
   { // Client
      close(up_pipe[0]);
      close(down_pipe[1]);

      // read the message from the user
      char msg[MAX_MSG_SIZE];
      printf("Enter a message: ");
      fgets(msg, MAX_MSG_SIZE, stdin);

      // send the message to the server to be processed and print it once processed
      write(up_pipe[1], msg, strlen(msg) + 1);
      read(down_pipe[0], msg, MAX_MSG_SIZE);
      printf("Received message: %s", msg);

      close(up_pipe[1]);
      close(down_pipe[0]);
   }
   else
   { // Server
      close(up_pipe[1]);
      close(down_pipe[0]);

      // read the message from the client and process it
      char msg[MAX_MSG_SIZE];
      read(up_pipe[0], msg, MAX_MSG_SIZE);

      // process the message - for each character, if it is a lower case letter, convert it to upper case, and vice versa
      for (int i = 0; i < strlen(msg); i++)
      {
         if (islower(msg[i]))
         {
            msg[i] = toupper(msg[i]);
         }
         else if (isupper(msg[i]))
         {
            msg[i] = tolower(msg[i]);
         }
      }

      // send the processed message back to the client
      write(down_pipe[1], msg, strlen(msg) + 1);
      close(up_pipe[0]);
      close(down_pipe[1]);
   }

   return 0;
}