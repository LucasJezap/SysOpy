#include "semaphore.h"

sem_t* makeSemaphore() {
    sem_t *semaphoreId;

    if ((semaphoreId = sem_open(SEMAPHORE_NAME,O_CREAT,0666,1)) == (sem_t *) -1) {
        printf("Error while creating semaphore!\n");
    }

    return semaphoreId;
}

sem_t* accessSemaphore() {
    sem_t* semaphoreId;

    if ((semaphoreId = sem_open(SEMAPHORE_NAME,0,0666,0)) == (sem_t *) -1) {
        printf("Error while accessing semaphore!\n");
    }

    return semaphoreId;
}

void closeSemaphore(sem_t* semaphoreId) {
    if (sem_close(semaphoreId) == -1) {
        printf("Error while closing semaphore!\n");
    }
}

void deleteSemaphore() {
    if (sem_unlink(SEMAPHORE_NAME) == -1) {
        printf("Error while deleting semaphore!\n");
    }
}

void increment(sem_t* semaphoreId) {
    if (sem_post(semaphoreId) == -1) {
        printf("Error while incrementing semaphore!\n");
    }
}

void decrement(sem_t* semaphoreId) {
    if (sem_wait(semaphoreId) == -1) {
        printf("Error while decrementing semaphore!\n");
    }
}