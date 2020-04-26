#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>

/* semaphore access key */
#define SEMAPHORE_KEY ftok(getenv("HOME"),'S')

/* pretty self-explanatory function names */
int makeSemaphore();
int accessSemaphore();
void deleteSemaphore(int semaphoreId);
void increment(int semaphoreId);
void decrement(int semaphoreId);