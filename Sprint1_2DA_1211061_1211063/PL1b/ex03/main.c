#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>

int main()
{
   // Print the list of pids running and the name of the process
   printf("The list of pids running are: \n");
   system("ps -A -o pid,comm");

   // Get the PID of the process to send the signal to
   int requestedPid;
   printf("Enter the PID of the process you want to send a signal to: ");
   scanf("%d", &requestedPid);

   // Get the signal to send
   int signal;
   printf("Enter the signal you want to send: ");
   scanf("%d", &signal);

   // Send the signal to the process
   if (kill(requestedPid, signal) == -1)
   {
      if (errno == EPERM)
      {
         // Print error message if the user does not have permission to send the signal
         printf("You do not have permission to send a signal to process %d\n", requestedPid);
      }
   }
   else
   {
      // Print final message
      printf("Signal sent to process %d with signal %d\n", requestedPid, signal);
   }

   return 0;
}