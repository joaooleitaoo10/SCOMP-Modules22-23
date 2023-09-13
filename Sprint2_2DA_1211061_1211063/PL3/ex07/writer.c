#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/shm.h>

#define STR_SIZE 50
#define NR_DISC 10

struct aluno
{
   int numero;
   char nome[STR_SIZE];
   int disciplinas[NR_DISC];
   int canRead;
   int canWrite;
};

int main()
{
   int fd, data_size = sizeof(struct aluno);
   struct aluno *myStudent;

   // Create shared memory area
   fd = shm_open("/shm_space", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
   if (fd == -1)
   {
      perror("Error opening file for writing");
      exit(EXIT_FAILURE);
   }
   if (ftruncate(fd, data_size) == -1)
   {
      perror("Error calling ftruncate()");
      exit(EXIT_FAILURE);
   }
   myStudent = (struct aluno *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
   if (myStudent == MAP_FAILED)
   {
      perror("Error mmapping the file");
      exit(EXIT_FAILURE);
   }
   // END Create shared memory area

   // Create the First Child
   int pid = fork();
   if (pid == 0)
   {
      while (myStudent->canRead == 0)
         ;

      // Calculate the max and min grades
      int max = myStudent->disciplinas[0];
      int min = myStudent->disciplinas[0];
      for (int i = 1; i < NR_DISC; i++)
      {
         if (myStudent->disciplinas[i] > max)
         {
            max = myStudent->disciplinas[i];
         }
         if (myStudent->disciplinas[i] < min)
         {
            min = myStudent->disciplinas[i];
         }
      }

      // Print the results
      printf("Max grade: %d\n", max);
      printf("Min grade: %d\n", min);

      // End the process
      exit(0);
   }

   // Create the Second Child
   pid = fork();
   if (pid == 0)
   {
      while (myStudent->canRead == 0)
         ;

      // Calculate the average grade
      int sum = 0;
      for (int i = 0; i < NR_DISC; i++)
      {
         sum += myStudent->disciplinas[i];
      }

      // Print the results
      printf("Average grade: %d\n", sum / NR_DISC);

      // End the process
      exit(0);
   }

   // Fill the shared memory in accordance with user-entered information
   printf("Enter student number: ");
   scanf("%d", &myStudent->numero);
   printf("Enter student name: ");
   scanf("%s", myStudent->nome);
   printf("Enter student grades: ");
   for (int i = 0; i < NR_DISC; i++)
   {
      scanf("%d", &myStudent->disciplinas[i]);
   }
   myStudent->canRead = 1;

   // Wait for the children to finish
   wait(NULL);
   wait(NULL);

   // Delete shared memory area
   if (munmap(myStudent, data_size) == -1)
   {
      perror("Error un-mmapping the file");
   }
   close(fd);
   shm_unlink("/shm_space");

   return 0;
}