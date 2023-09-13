#include <semaphore.h>

int insert();
int consult();
int consultAll();
void up(sem_t *sem);
void down(sem_t *sem);