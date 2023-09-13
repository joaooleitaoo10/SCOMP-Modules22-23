#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

typedef struct
{
   int array[10];
   int canRead;
} SharedResource;

int main()
{
   int fd, data_size = sizeof(SharedResource);
   SharedResource *mySharedResourcePtr;

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

   mySharedResourcePtr = (SharedResource *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
   if (mySharedResourcePtr == MAP_FAILED)
   {
      perror("Error mmapping the file");
      exit(EXIT_FAILURE);
   }

   srand(time(0));
   // Fill the array with 10 random numbers (1-20)
   for (int i = 0; i < 10; i++)
   {
      mySharedResourcePtr->array[i] = rand() % 20 + 1;
   }

   // Set the canRead flag to 1
   mySharedResourcePtr->canRead = 1;

   return 0;
}