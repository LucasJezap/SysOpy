#include "sharedCounter.h"

int makeCounter() {
    int counterId;

    if ((counterId = shmget(COUNTER_KEY,sizeof(OrderCounter),IPC_CREAT | 0666)) == -1) {
        printf("Error while creating shared memory counter!\n");
    }

    return counterId;
}

int accessCounter() {
    int counterId;

    if ((counterId = shmget(COUNTER_KEY,0,0666)) == -1) {
        printf("Error while accessing shared memory counter!\n");
    }

    return counterId;
}

OrderCounter* getCounter(int counterId) {
    OrderCounter *counter;

    if ((counter = shmat(counterId,NULL,0)) == (void *) -1) {
        printf("Error while getting shared memory counter!\n");
    }

    return counter;
}

void closeCounter(OrderCounter *counter) {
    if(shmdt(counter) == -1) {
        printf("Error while closing shared memory counter!\n");
    }
}

void deleteCounter(int counterId) {
    if(shmctl(counterId,IPC_RMID,NULL) == -1) {
        printf("Error while deleting shared memory counter!\n");
    }
}