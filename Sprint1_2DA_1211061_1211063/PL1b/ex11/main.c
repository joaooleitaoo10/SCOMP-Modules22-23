#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void handler_SIGUSR1(int sigNum)
{
   write(STDOUT_FILENO, "SIGUSR1 received", 16);
}

int main()
{
   sigset_t mask, pending;

   // Block all the signals except SIGUSR1
   sigfillset(&mask);
   sigdelset(&mask, SIGUSR1);

   // Set the mask
   sigprocmask(SIG_SETMASK, &mask, NULL);

   // Set the signal handler for SIGUSR1
   struct sigaction handler;
   memset(&handler.sa_mask, 0, sizeof(handler.sa_mask));
   sigemptyset(&handler.sa_mask);

   handler.sa_handler = handler_SIGUSR1;
   sigaction(SIGUSR1, &handler, NULL);

   // Wait for the signal to be received
   pause();

   // Get the pending signals
   sigpending(&pending);

   // List all the blocked signals
   for (int i = 1; i <= 64; i++)
   {
      if (sigismember(&pending, i))
      {
         printf("\nSignal %d is blocked\n", i);
      }
   }
}