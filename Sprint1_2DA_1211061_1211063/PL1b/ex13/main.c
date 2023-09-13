#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

// method to perform task A
void taskA()
{
   printf("Task A: started!\n");
   sleep(3);
   printf("Task A: done!\n");
}

// method to perform task B
void taskB()
{
   srand(time(NULL));
   int random_time = rand() % 5 + 1;
   printf("Task B: started!\n");
   sleep(random_time);
   printf("Task B: done!\n");
}

// method to perform task C
void taskC()
{
   printf("Task C: started!\n");
   sleep(1);
   printf("Task C: done!\n");
}

// signal handler for SIGUSR1
void handler_SIGUSR1(int signum)
{
   taskB();
   taskC();
}

int main()
{
   pid_t pid;
   int status;

   // Create a child process
   pid = fork();

   if (pid == 0)
   {
      struct sigaction act;
      memset(&act.sa_mask, 0, sizeof(act.sa_mask));
      sigfillset(&act.sa_mask);

      // logic to handle SIGUSR1
      act.sa_handler = handler_SIGUSR1;
      sigaction(SIGUSR1, &act, NULL);

      pause();

      exit(0);
   }
   else
   { // parent process
      taskA();
      kill(pid, SIGUSR1);
      waitpid(pid, &status, 0);
      printf("Job is complete!\n");
   }

   return 0;
}