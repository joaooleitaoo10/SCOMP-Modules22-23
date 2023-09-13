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

#define NR_INTS 10
#define NR_EXCHANGED 30

typedef struct
{
   int numbers[NR_INTS];
   int writerIdx;
   int readerIdx;
} Circular_buffer;

int main()
{
   int fd, data_size = sizeof(Circular_buffer);
   Circular_buffer *myBuffer;

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
   myBuffer = (Circular_buffer *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
   if (myBuffer == MAP_FAILED)
   {
      perror("Error mmapping the file");
      exit(EXIT_FAILURE);
   }
   // END Create shared memory area

   myBuffer->readerIdx = 0;

   // Create reader process
   int pid = fork();
   if (pid == -1)
   {
      perror("Error creating child process");
      exit(EXIT_FAILURE);
   }
   else if (pid == 0)
   {
      int i;
      for (i = 0; i < NR_EXCHANGED; i++)
      {
         // Wait for writer to write
         while (myBuffer->readerIdx == myBuffer->writerIdx)
            ;

         // Read
         printf("\e[1;32mI Read: %d\n", myBuffer->numbers[myBuffer->readerIdx]);
         // Update reader index
         if (myBuffer->readerIdx == NR_INTS - 1)
            myBuffer->readerIdx = 0;
         else
         {
            myBuffer->readerIdx = (myBuffer->readerIdx + 1);
         }
      }
      exit(EXIT_SUCCESS);
   }

   // Writer
   int i;
   for (i = 0; i < NR_EXCHANGED; i++)
   {
      while (((myBuffer->writerIdx + 1) % NR_INTS) == myBuffer->readerIdx && i != 0)
         ;

      // Write
      myBuffer->numbers[myBuffer->writerIdx] = i;

      printf("\e[0;35mI wrote: %d\n", myBuffer->numbers[myBuffer->writerIdx]);

      // Update writer index
      if (myBuffer->writerIdx == NR_INTS - 1)
         myBuffer->writerIdx = 0;
      else
      {
         myBuffer->writerIdx = (myBuffer->writerIdx + 1);
      }
   }

   // Delete shared memory area
   if (munmap(myBuffer, data_size) == -1)
   {
      perror("Error un-mmapping the file");
   }
   close(fd);
   shm_unlink("/shm_space");
   wait(NULL);

   return 0;
}
