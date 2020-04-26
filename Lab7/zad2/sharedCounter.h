#define _XOPEN_SOURCE 500
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

/* shared memory counter access key */
#define COUNTER_NAME "/counter"

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
void deleteCounter();