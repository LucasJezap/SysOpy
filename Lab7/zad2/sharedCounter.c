#include "sharedCounter.h"

int makeCounter() {
    int counterId;

    if ((counterId = shm_open(COUNTER_NAME, O_CREAT | O_EXCL | O_RDWR, 0666)) == -1) {
        printf("Error while creating shared memory counter!\n");
    }
    
    if (ftruncate(counterId,sizeof(OrderCounter)) == -1) {
        printf("Error while reserving shared memory counter space!\n");
    }

    return counterId;
}

int accessCounter() {
    int counterId;

    if ((counterId = shm_open(COUNTER_NAME,O_RDWR,0666)) == -1) {
        printf("Error while accessing shared memory counter!\n");
    }

    return counterId;
}

OrderCounter* getCounter(int counterId) {
    OrderCounter *counter;

    if ((counter = mmap(NULL,sizeof(OrderCounter),PROT_READ | PROT_WRITE, MAP_SHARED, counterId,0)) == (void *) -1) {
        printf("Error while getting shared memory counter!\n");
    }

    return counter;
}

void closeCounter(OrderCounter *counter) {
    if(munmap(counter,sizeof(OrderCounter)) == -1) {
        printf("Error while closing shared memory counter!\n");
    }
}

void deleteCounter() {
    if(shm_unlink(COUNTER_NAME) == -1) {
        printf("Error while deleting shared memory counter!\n");
    }
}