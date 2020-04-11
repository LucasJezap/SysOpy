#define _GNU_SOURCE
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <string.h>
#include <sys/msg.h>

/* limit of clients and max size of message */
#define CLIENT_LIMIT 20
#define MAX_MSG_SIZE 4096

/* path and proj helpful for ftok */
#define PROJ 'A'
#define PATH "/home"

/* implemented error for throwing */

void ERROR(char *msg) { 
    printf("%s\n",msg);
    exit(-5);
}

/* own designed mtype need for system V communication */
typedef enum mtype {
    INIT = 1,
    STOP = 2,
    DISCONNECT = 3,
    LIST = 4,
    CONNECT = 5,
    CHAT = 6
} mtype;

typedef struct client {
    pid_t client_pid;
    int disconnected;
    int queue_key;
    int queue_id;
    int client_id;
    int active;
} client;

/* message designed for system V communication */
typedef struct message {
    long mtype;
    int client_id;
    int pair_id;
    int queue_key;
    int queue_id;
    pid_t client_pid;
    char message[MAX_MSG_SIZE];

} message;

/* msgsz value need for msgsnd() */
const size_t MSGSZ = sizeof(message) - sizeof(long);

#endif // STRUCTURES_H