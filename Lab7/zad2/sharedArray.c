#include "sharedArray.h"
#include <errno.h>

int makeArray() {
    int arrayId;

    if ((arrayId = shm_open(ARRAY_NAME, O_CREAT | O_EXCL | O_RDWR, 0666)) == -1) {
        printf("Error while creating shared memory array!\n");
    }

    if (ftruncate(arrayId,sizeof(ShopOrder)*MAX_ARRAY_SIZE) == -1) {
        printf("Error while reserving shared memory array space!\n");
    }

    return arrayId;
}

int accessArray() {
    int arrayId;

    if ((arrayId = shm_open(ARRAY_NAME,O_RDWR,0666)) == -1) {
        printf("Error while accessing shared memory array!\n");
    }

    return arrayId;
}

ShopOrder* getArray(int arrayId) {
    ShopOrder* shopOrders;

    if ((shopOrders = mmap(NULL,sizeof(ShopOrder)*MAX_ARRAY_SIZE,PROT_READ | PROT_WRITE,MAP_SHARED,arrayId,0)) == (void *) -1) {
        printf("Error while getting shared memory array!\n");
    }

    return shopOrders;
}

void closeArray(ShopOrder *shopOrders) {
    if(munmap(shopOrders,sizeof(ShopOrder)*MAX_ARRAY_SIZE) == -1) {
        printf("Error while closing shared memory array!\n");
    }
}

void deleteArray() {
    if(shm_unlink(ARRAY_NAME) == -1) {
        printf("Error while deleting shared memory array!\n");
    }
}

int firstEmpty(int i, ShopOrder *shopOrders) {
    int cnt = 0;
    while (cnt++ < MAX_ARRAY_SIZE) {
        if (shopOrders[i].order_number == 0)
            return i;

        i = (i+1)%MAX_ARRAY_SIZE;
    }
    return -1;
}

int firstUnpacked(int i, ShopOrder *shopOrders) {
    int cnt = 0;
    while (cnt++ < MAX_ARRAY_SIZE) {
        if (shopOrders[i].is_waiting)
            return i;

        i = (i+1)%MAX_ARRAY_SIZE;
    }
    return -1;
}

int firstPacked(int i, ShopOrder *shopOrders) {
    int cnt = 0;
    while (cnt++ < MAX_ARRAY_SIZE) {
        if (shopOrders[i].is_packed)
            return i;

        i = (i+1)%MAX_ARRAY_SIZE;
    }
    return -1;
}

char* get_time() {
    char buffer[26];
    int millisec;
    struct tm* tm_info;
    struct timeval tv;

    gettimeofday(&tv,NULL);

    millisec = lrint(tv.tv_usec/1000.0);
    if (millisec >= 1000) {
        millisec -= 1000;
        tv.tv_sec++;
    }

    tm_info = localtime(&tv.tv_sec);

    strftime(buffer,26,"%Y:%m:%d %H:%M:%S", tm_info);

    char *time = (char *)calloc(40,sizeof(char));

    sprintf(time,"%s.%03d",buffer,millisec);

    return time;
}