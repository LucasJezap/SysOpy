#include "semaphore.h"
#include "sharedArray.h"
#include "sharedCounter.h"

/* global variables */
int WORKERS;
pid_t *pids;
sem_t* semaphoreId;
int counterId, arrayId;
ShopOrder* shopOrders;
OrderCounter *counter;
int arrayIndex = 0;

/* handler for SIGINT signal, sends SIGUSR1 to every employee */
void handler(int signum) {
    printf("\nShop is going to close, every employee should go home!\n");
    for (int i=0; i<WORKERS; i++)
        kill(pids[i],SIGUSR1);
}

/* program symbolizing shop, spawning appropriate workers in the shop */
int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Wrong number of arguments!\n");
        return -1;
    }
    
    struct timeval tv;
    gettimeofday(&tv,NULL);
    srand(lrint(tv.tv_usec));

    /* creating appropriate shared memory objects and semaphore */
    arrayId = makeArray();
    counterId = makeCounter();
    semaphoreId = makeSemaphore();

    counter = getCounter(counterId);
    shopOrders = getArray(arrayId);

    /* setting appropriate starting values to 0 */
    counter->packed_orders = counter->unpacked_orders = 0;

    for(int i=0; i<MAX_ARRAY_SIZE; i++) {
        shopOrders[i].order_number = shopOrders[i].is_packed = shopOrders[i].is_waiting = 0;
    }

    /* randomly choosing number of each worker type */
    int workers[] = {atoi(argv[1]), atoi(argv[2]), atoi(argv[3])};
    WORKERS = workers[0] + workers[1] + workers[2];
    pids = (pid_t *)calloc(WORKERS,sizeof(pid_t));

    printf("In the company there are %d receivers, %d packers and %d senders\n",workers[0],workers[1],workers[2]);

    /* spawning workers */
    int worker_num = 0;
    
    for(int i=0; i<workers[0]; i++) {
        if((pids[worker_num++] = fork()) == 0) {
                execl("./receiver","./receiver",NULL);
        }
    }

    for(int i=0; i<workers[1]; i++) {
        if((pids[worker_num++] = fork()) == 0) {
                execl("./packer","./packerr",NULL);
        }
    }

    for(int i=0; i<workers[1]; i++) {
        if((pids[worker_num++] = fork()) == 0) {
                execl("./sender","./sender",NULL);
        }
    }

    /* setting handler and waiting for workers to end */
    signal(SIGINT,handler);

    for(int i=0; i<WORKERS; i++) {
        waitpid(pids[i],NULL,0);
    }

    /* closing all shared memory objects and semaphore */
    closeArray(shopOrders);
    closeCounter(counter);
    closeSemaphore(semaphoreId);
    deleteArray();
    deleteCounter();
    deleteSemaphore();
    free(pids);

    return 0;
}