#include "semaphore.h"
#include "sharedCounter.h"
#include "sharedArray.h"

/* global variables */
sem_t* semaphoreId;
int counterId, arrayId;
ShopOrder* shopOrders;
OrderCounter *counter;
int arrayIndex = 0;

/* handler for SIGUSR1 signal */
void handler(int signum) {
    closeArray(shopOrders);
    closeCounter(counter);
    closeSemaphore(semaphoreId);
    exit(0);
}

/* program symbolizing packer worker */
int main() {
    /* setting SIGUSR1 handler and accessing shared memory objects and semaphore */
    signal(SIGUSR1,handler);

    arrayId = accessArray();
    counterId = accessCounter();
    semaphoreId = accessSemaphore();

    shopOrders = getArray(arrayId);
    counter = getCounter(counterId);

    /* packer starts to work */
    while(1 == 1) {
        struct timeval tv;
        gettimeofday(&tv,NULL);
        srand(lrint(tv.tv_usec));
        
        usleep(1000*(rand()%1000+1000));

        /* decrement semaphore so only packer will work at given time */
        decrement(semaphoreId);

        /* if there are any unpacked orders */
        if (counter->unpacked_orders > 0) {
            /* get appropriate order, pack it and give info to the shop */
            arrayIndex = firstUnpacked(arrayIndex,shopOrders);

            shopOrders[arrayIndex].order_number *= 2;
            shopOrders[arrayIndex].is_waiting = 0;
            shopOrders[arrayIndex].is_packed = 1;
            
            counter->unpacked_orders--;
            counter->packed_orders++;

            char *time = get_time();

            printf("(%d %s) PACKER: I've packed order of size: %d, Number of orders to pack: %d, Number of orders to send: %d\n"
                    ,getpid(),time,shopOrders[arrayIndex].order_number,counter->unpacked_orders,counter->packed_orders);

            arrayIndex = (arrayIndex + 1)%MAX_ARRAY_SIZE;
        }

        /* increment semaphore back after operation */
        increment(semaphoreId);
    }

}