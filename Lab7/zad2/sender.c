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

/* program symbolizing sender worker */
int main() {
    /* setting SIGUSR1 handler and accessing shared memory objects and semaphore */
    signal(SIGUSR1,handler);

    arrayId = accessArray();
    counterId = accessCounter();
    semaphoreId = accessSemaphore();

    shopOrders = getArray(arrayId);
    counter = getCounter(counterId);

    /* sender starts to work */
    while(1 == 1) {
        struct timeval tv;
        gettimeofday(&tv,NULL);
        srand(lrint(tv.tv_usec));
        
        usleep(1000*(rand()%1000+1000));

        /* decrement semaphore so only sender will work at given time */
        decrement(semaphoreId);

        /* if there are any packed orders */
        if (counter->packed_orders > 0) {
            /* get appropriate order, send it and give info to the shop */
            arrayIndex = firstPacked(arrayIndex,shopOrders);

            shopOrders[arrayIndex].order_number *= 3;
            shopOrders[arrayIndex].is_packed = 0;
            
            counter->packed_orders--;

            char *time = get_time();

            printf("(%d %s) SENDER: I've sent order of size: %d, Number of orders to pack: %d, Number of orders to send: %d\n"
                    ,getpid(),time,shopOrders[arrayIndex].order_number,counter->unpacked_orders,counter->packed_orders);

            arrayIndex = (arrayIndex + 1)%MAX_ARRAY_SIZE;
        }

        /* increment semaphore back after operation */
        increment(semaphoreId);
    }

}