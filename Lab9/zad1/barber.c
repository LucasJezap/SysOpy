#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define SLEEP_THREAD 1
#define SLEEP_BARBER_JOB 4
#define SLEEP_FULL_SHOP 2

#define RED() (printf("\033[1;31m"))
#define YELLOW() (printf("\033[1;33m"))
#define GREEN() (printf("\033[1;32m"))
#define DEFAULT() (printf("\033[0m"))

/* Global variables */
int number_of_chairs, number_of_clients;
pthread_t barber;
pthread_t *clients;
int *waiting_room;
int *time_of_wait;
int barber_working = 0;
int current_clients = 0;
int done_today = 0;
int start = 1;
pthread_mutex_t office = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

/* Auxilliary functions */
void throw_error(char *msg);
int get_client_id(pthread_t client);
void* client_thread();
void* barber_thread();
int full_room();
int empty_room();
int find_next_free_place();
int find_next_client(int chair_num);
void show_chairs();
void increment_wait(int chair_id);

/**
 * Program symbolizing barber shop 
 */
int main(int argc, char *argv[]) {
    /* checking and parsing arguments */
    if (argc != 3) {
        throw_error("Invalid number of arguments!");
        return -1;
    }

    if ((number_of_chairs = atoi(argv[1])) < 0) {
        throw_error("Wrong number of chairs parameter!");
        return -1;
    }

    if ((number_of_clients = atoi(argv[2])) < 0) {
        throw_error("Wrong number of clients parameter!");
        return -1;
    }
    /* initializing structures */
    clients = (pthread_t *)calloc(number_of_clients,sizeof(pthread_t));
    waiting_room = (int *)calloc(number_of_chairs,sizeof(int));
    time_of_wait = (int *)calloc(number_of_chairs,sizeof(int));

    for (int i=0; i<number_of_chairs; i++) {
        waiting_room[i] = -1;
        time_of_wait[i] = -1;
    }

    /* welcome messages */
    GREEN();
    printf("The barber shop has %d chairs in waiting room and %d clients are going to visit the shop today.\n",number_of_chairs,number_of_clients);
    printf("All clients are getting cut in the same order they sat on their chairs.\n");
    printf("For example if Client 3 couldn't sit on any chair and then Client 4 came before him then he will be cut first.\n\n");
    DEFAULT();

    /* creating barber */
    pthread_create(&barber,NULL,barber_thread,NULL);

    /* creating clients */
    for (int i=0; i<number_of_clients; i++) {
        srand(time(NULL));
        sleep(1+rand()%SLEEP_THREAD);
        pthread_create(&clients[i],NULL,client_thread,NULL);
    }

    /* waiting for clients to be done */
    for (int i=0; i<number_of_clients; i++) {
        pthread_join(clients[i],NULL);
    }

    /* waiting for barber to end */
    pthread_join(barber,NULL);
    
    /* closing message */
    GREEN();
    printf("Today the barber managed to cut beards of %d clients, congrats!\n",done_today);
    DEFAULT();

    /* freeing allocated space */
    free(clients);
    free(waiting_room);
    free(time_of_wait);

    return 0;
}

/**
 * Function throwing given error message 
 */
void throw_error(char *msg) {
    printf("Error: %s\n",msg);
}

/**
 * Function returning client's thread id
 */
int get_client_id(pthread_t client) {
    for (int i=0; i<number_of_clients; i++) {
        if (pthread_equal(client,clients[i]))
            return i;
    }
    return -1;
}

/**
 * Function returning 1 if shop is full of clients, 0 otherwise
 */
int full_room() {
    return current_clients == number_of_chairs;
}

/**
 * Function returning 1 if shop is empty, 0 otherwise
 */
int empty_room() {
    return current_clients == 0;
}

/**
 * Function returning number of next free chair in waiting room
 */
int find_next_free_place() {
    if (full_room())
        return -1;
    
    int i=0;
    while(waiting_room[i] != -1) {
        i++;
    }

    return i;
}

/**
 * Function returning number of next free chair in waiting room
 */
int find_next_client(int chair_num) {
    if (empty_room())
        return -1;
    
    int i = 0;

    for(int j=1; j<number_of_chairs; j++){
        if(waiting_room[j] != -1 && time_of_wait[j] > time_of_wait[i])
            i=j;
    }

    return i;
}

/**
 * Helping function showing chairs if number of chairs is small
 */
void show_chairs() {
    if (number_of_chairs > 50)
        return;
    
    for (int i=0; i<number_of_chairs; i++)
        printf("%-2d ",i);
    printf("\n");
    for (int i=0; i<number_of_chairs; i++) {
        if (waiting_room[i] == -1)
            printf("%-2s ","o");
        else
            printf("%-2s ","x");
    }
    printf("\n\n");
}

/**
 * Function incrementing time of wait for all current customers
 */
void increment_wait(int chair_id) {
    for (int i=0; i<number_of_chairs; i++) {
        if (waiting_room[i] != -1 && i != chair_id)
            time_of_wait[i]++;
    }
}

/**
 * Function symbolizing client in barber shop
 */
void* client_thread() {
    int client_id = get_client_id(pthread_self());

    if (number_of_chairs == 0) {
        YELLOW();
        printf("Client %d: Seems like barber forgot to put some chairs, maybe tomorrow!\n\n",client_id);
        DEFAULT();
        pthread_exit(0);
    }

    while(full_room()) {
        YELLOW();
        printf("Client %d: The barber shop is full! I'll try later\n\n",client_id);
        DEFAULT();
        srand(time(NULL));
        sleep(1+rand()%SLEEP_FULL_SHOP);
    }

    pthread_mutex_lock(&office);

    int chair_id = find_next_free_place();
    current_clients++;
    waiting_room[chair_id] = client_id;
    time_of_wait[chair_id] = 0;
    increment_wait(chair_id);

    YELLOW();
    if (barber_working) {
        printf("Client %d: The barber is working, i'll take chair %d; Empty chairs: %d\n",client_id,chair_id,number_of_chairs-current_clients);
    }

    else {
        printf("Client %d: The barber is sleeping, i'll wake him up and get cut: Empty chairs: %d\n",client_id,number_of_chairs-current_clients);
        pthread_cond_broadcast(&empty);
    }
    DEFAULT();

    show_chairs();
    pthread_mutex_unlock(&office);

    pthread_exit(0);
}

/**
 * Function symbolizing barber in barber shop
 */
void* barber_thread() {
    int chair_num = 0;

    while(number_of_chairs != 0 && done_today != number_of_clients) {
        
        pthread_mutex_lock(&office);
        int tmp = empty_room();
        if (tmp) {
            RED();
            printf("Barber: I'm going to sleep...\n\n");
            DEFAULT();
            barber_working = 0;
        }
        while(empty_room()) {
            pthread_cond_wait(&empty,&office);    
        }
        barber_working = 1;
        chair_num = find_next_client(chair_num);
        current_clients--;
        RED();
        printf("Barber: I'm cutting beard of Client %d; Clients waiting: %d\n",waiting_room[chair_num],current_clients);
        DEFAULT();
        tmp = waiting_room[chair_num];
        waiting_room[chair_num] = -1;
        time_of_wait[chair_num] = -1;

        show_chairs();
        pthread_mutex_unlock(&office);

        srand(time(NULL));
        sleep(1+rand()%SLEEP_BARBER_JOB);
        RED();
        printf("Barber: I'm done with Client %d\n",tmp);
        DEFAULT();
        done_today++;
    }

    pthread_exit(0);
}