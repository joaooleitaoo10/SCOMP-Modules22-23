 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <sys/mman.h>
 #include <fcntl.h>
 #include <string.h>
 
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
 
    fd = shm_open("/shm_ex01", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
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
 
    myStudentPtr = (Student *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (myStudentPtr == MAP_FAILED)
    {
       perror("Error mmapping the file");
       exit(EXIT_FAILURE);
    }
 
    myStudentPtr->number = 1234;
    strcpy(myStudentPtr->name, "Lightning McQueen");
    strcpy(myStudentPtr->addy, "Radiator Springs");
 
    return 0;
 }