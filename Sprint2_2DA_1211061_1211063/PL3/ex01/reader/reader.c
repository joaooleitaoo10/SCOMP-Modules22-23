#include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <sys/mman.h>
 #include <fcntl.h>
 
 typedef struct
 {
    int number;
    char name[20];
    char addy[30];
 } Student;
 
 int main()
 {
    int fd, data_size = sizeof(Student);
    Student *myStudentPtr;
 
    fd = shm_open("/shm_ex01", O_RDWR, S_IRUSR | S_IWUSR);
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
 
    myStudentPtr = (Student *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (myStudentPtr == MAP_FAILED)
    {
       perror("Error mmapping the file");
       exit(EXIT_FAILURE);
    }
 
    // Active wait for data to be written
    while (myStudentPtr->number == 0)
       ;
 
    printf("Student number: %d\n", myStudentPtr->number);
    printf("Student name: %s\n", myStudentPtr->name);
    printf("Student address: %s\n", myStudentPtr->addy);

   // remove the shared memory object
   if (shm_unlink("/shm_ex01") == -1)
   {
      perror("Error removing /shm_ex01");
      exit(EXIT_FAILURE);
   }

    return 0;
 }