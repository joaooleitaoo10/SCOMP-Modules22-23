#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>

#include "header.h"

int main()
{
    int option;
    while (1)
    {

        printf("\n-----MENU-----");
        printf("\n1 - Insert");
        printf("\n2 - Consult");
        printf("\n3 - Consult All");
        printf("\n\n0 - Exit");
        printf("\n\nOption: ");
        scanf("%d%*c", &option);

        switch (option)
        {
        case 1:
            insert();
            break;
        case 2:
            consult();
            break;
        case 3:
            consultAll();
            break;
        case 0:
            printf("\nExiting...\n");
            shm_unlink("/shmtest");
            sleep(2);
            return 0;
        default:
            printf("\nInvalid Option!\n");
            break;
        }
    }

    return 0;
}