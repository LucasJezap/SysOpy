#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

/* semaphore access key */
#define SEMAPHORE_NAME "/sem"

/* pretty self-explanatory function names */
sem_t* makeSemaphore();
sem_t* accessSemaphore();
void closeSemaphore(sem_t* semaphoreId);
void deleteSemaphore();
void increment(sem_t* semaphoreId);
void decrement(sem_t* semaphoreId);