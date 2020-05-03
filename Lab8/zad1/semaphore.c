#include "semaphore.h"
#include <errno.h>

int makeSemaphore() {
    int semaphoreId;

    if ((semaphoreId = semget(SEMAPHORE_KEY,1,IPC_CREAT | 0666)) == -1) {
        printf("Error while creating semaphore!\n");
    }

    increment(semaphoreId);
    
    return semaphoreId;
}

int accessSemaphore() {
    int semaphoreId;

    if ((semaphoreId = semget(SEMAPHORE_KEY,0,0666)) == -1) {
        printf("Error while accessing semaphore!\n");
    }

    
    return semaphoreId;
}

void deleteSemaphore(int semaphoreId) {
    if (semctl(semaphoreId,0,IPC_RMID) == -1) {
        printf("Error while deleting semaphore!\n");
    }
}

void increment(int semaphoreId) {
    struct sembuf buffers[1];
    buffers[0].sem_num = 0;
    buffers[0].sem_flg = 0;
    buffers[0].sem_op = 1;
    
    if (semop(semaphoreId,buffers,1) == -1) {
        printf("ERRNO = %d",errno);
        printf("Error while incrementing semaphore!\n");
    }
}

void decrement(int semaphoreId) {
    struct sembuf buffers[1];
    buffers[0].sem_num = 0;
    buffers[0].sem_flg = 0;
    buffers[0].sem_op = -1;
    
    if (semop(semaphoreId,buffers,1) == -1) {
        printf("Error while decrementing semaphore!\n");
    }
}