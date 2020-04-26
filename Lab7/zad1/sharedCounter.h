#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

/* shared memory counter access key */
#define COUNTER_KEY ftok(getenv("HOME"),'C')

/* structure symbolizing shop counter of orders */
struct OrderCounter {
    int packed_orders;
    int unpacked_orders;
} typedef OrderCounter;

/* pretty self-explanatory function names */
int makeCounter();
int accessCounter();
OrderCounter* getCounter(int counterId);
void closeCounter(OrderCounter *counter);
void deleteCounter(int counterId);