#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void handler_SIGINT(int sigNum)
{
   write(STDOUT_FILENO, "I won’t let the process end with CTRL-C!", 42);
}

void handler_SIGQUIT(int sigNum)
{
   write(STDOUT_FILENO, "I won’t let the process end with CTRL-\\!", 42);
}

int main()
{
   struct sigaction act;
   memset(&act.sa_mask, 0, sizeof(act.sa_mask));
   sigemptyset(&act.sa_mask);

   // logic to handle SIGINT
   act.sa_handler = handler_SIGINT;
   sigaction(SIGINT, &act, NULL);

   // logic to handle SIGQUIT
   act.sa_handler = handler_SIGQUIT;
   sigaction(SIGQUIT, &act, NULL);

   for (;;)
   {
      printf("\nI like signals\n");
      sleep(1);
   }
}