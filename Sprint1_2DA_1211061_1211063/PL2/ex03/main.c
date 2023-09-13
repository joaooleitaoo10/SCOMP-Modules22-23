#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>

#define MSGSIZE 16

char *msg1 = "Hello World";
char *msg2 = "Goodbye!";

int main()
{
   char inbuf[MSGSIZE];
   int p[2], pid, nbytes;

   // Create the pipe and assure that it was created successfully.
   if (pipe(p) < 0)
   {
      exit(EXIT_FAILURE);
   }

   // Create a child process and assure that it was created successfully.
   if ((pid = fork()) < 0)
   {
      exit(EXIT_FAILURE);
   }

   // Parent process
   if (pid > 0)
   {
      // The parent process must close the read end of the pipe and write two messages to the pipe.
      close(p[0]);
      write(p[1], msg1, MSGSIZE);
      write(p[1], msg2, MSGSIZE);
      close(p[1]);
      printf("Parent: I wrote both messages successfuly!\n");

      // The parent process must wait for its child to end, printing its PID and exit value.
      int status;
      wait(&status);

      printf("Parent: My child process ended with PID %d and exit value %d \n", pid, WEXITSTATUS(status));
   }
   else
   {
      // The child process must close the write end of the pipe and read the two messages from the pipe.
      close(p[1]);
      while ((nbytes = read(p[0], inbuf, MSGSIZE)) > 0)
      {
         printf("Child: I received %s from the pipe.\n", inbuf);
      }

      // Exit with failure if the read() function returns an error (-1)
      if (nbytes != 0)
      {
         exit(EXIT_FAILURE);
      }

      printf("Child: All done here :)\n");
      close(p[0]);
   }
   return 0;
}