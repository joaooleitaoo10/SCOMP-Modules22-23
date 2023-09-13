#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
   int p[2], pid;

   if (pipe(p) < 0)
      exit(1);

   if ((pid = fork()) > 0)
   {
      printf("Parent process ID: %d\n", getpid());
      close(p[0]);
      pid = getpid();
      write(p[1], &pid, sizeof(pid));
      close(p[1]);
   }
   else
   {
      close(p[1]);
      read(p[0], &pid, sizeof(pid));
      printf("Parent process ID: %d\n", pid);
      close(p[0]);
   }

   return 0;
}