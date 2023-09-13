#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

typedef struct
{
   char cmd[32];
   int time_cap;
} command_t;

char *command;

void handler_SIGALRM(int signum)
{
   if (signum == SIGALRM)
   {
      printf("The command %s did not complete in its allowed time!\n", command);
      exit(EXIT_FAILURE);
   }
}

int main()
{
   command_t commands[] = {{"ls", 5}, {"sleep 10", 3}, {"echo 'Hello World!'", 15}};
   int commands_size = sizeof(commands) / sizeof(command_t);

   for (int i = 0; i < commands_size; i++)
   {
      // Execute the command with a time cap
      printf("\nExecuting command %s\n", commands[i].cmd);
      command = commands[i].cmd;
      pid_t child_pid = fork();
      if (child_pid == 0)
      {
         // Logic to handle SIGALRM
         struct sigaction act;
         memset(&act.sa_mask, 0, sizeof(act.sa_mask));
         sigemptyset(&act.sa_mask);
         act.sa_handler = handler_SIGALRM;
         sigaction(SIGALRM, &act, NULL);

         alarm(commands[i].time_cap);
         system(command);
         alarm(0);
         exit(EXIT_SUCCESS);
      }
      else
      {

         int status;
         waitpid(child_pid, &status, 0);

         // Check if the command exection was successful and not interrupted by SIGALRM
         if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)
         {
            printf("The command %s completed successfully!\n", command);
         }
      }
   }

   return 0;
}