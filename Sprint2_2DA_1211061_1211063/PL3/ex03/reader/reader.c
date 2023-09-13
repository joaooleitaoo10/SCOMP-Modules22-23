#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

typedef struct
{
   int array[10];
   int canRead;
} SharedResource;

int main()
{
   int fd, data_size = sizeof(SharedResource);
   SharedResource *mySharedResourcePtr;

   fd = shm_open("/shm_space", O_RDWR, S_IRUSR | S_IWUSR);
   if (fd == -1)
   {
      perror("Error opening file for reading");
      exit(EXIT_FAILURE);
   }

   if (ftruncate(fd, data_size) == -1)
   {
      perror("Error calling ftruncate()");
      exit(EXIT_FAILURE);
   }

   mySharedResourcePtr = (SharedResource *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
   if (mySharedResourcePtr == MAP_FAILED)
   {
      perror("Error mmapping the file");
      exit(EXIT_FAILURE);
   }

   // Wait until the canRead flag is set to 1
   while (mySharedResourcePtr->canRead == 0)
      ;

   // Read the 10 values, calculate and print the average
   int sum = 0;
   for (int i = 0; i < 10; i++)
   {
      printf("Value: %d\n", mySharedResourcePtr->array[i]);
      sum += mySharedResourcePtr->array[i];
   }
   int average = sum / 10;
   printf("Average: %d\n", average);

   return 0;
}