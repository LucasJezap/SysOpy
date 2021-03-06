#define _XOPEN_SOURCE 500
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

/* shared memory array access key and maximum array size */
#define ARRAY_NAME "/array"
#define MAX_ARRAY_SIZE 10

/* structure symbolizing a shop order */
struct ShopOrder {
    int order_number;
    int is_waiting;
    int is_packed;
} typedef ShopOrder;

/* pretty self-explanatory function names */
int makeArray();
int accessArray();
ShopOrder* getArray(int arrayId);
void closeArray(ShopOrder *shopOrders);
void deleteArray();

int firstEmpty(int i, ShopOrder *shopOrders);
int firstUnpacked(int i, ShopOrder *shopOrders);
int firstPacked(int i, ShopOrder *shopOrders);

char* get_time();