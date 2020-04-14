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
#include <fcntl.h>
#include <sys/msg.h>
#include <mqueue.h>

/* limit of clients and max size of message */
#define CLIENT_LIMIT 20
#define MAX_MSG_SIZE 4096

/* server path */
#define PATH "/server"

/* implemented error for throwing */

void ERROR(char *msg) { 
    printf("%s\n",msg);
    exit(-5);
}

/* own designed mtype need for system V communication */
typedef enum mtype {
    INIT = 6,
    STOP = 5,
    DISCONNECT = 4,
    LIST = 3,
    CONNECT = 2,
    CHAT = 1
} mtype;

typedef struct client {
    pid_t client_pid;
    int disconnected;
    int queue_id;
    int client_id;
    int active;
} client;

/* message designed for system V communication */
typedef struct message {
    long mtype;
    int client_id;
    int pair_id;
    char path[100];
    int queue_id;
    pid_t client_pid;
    char message[MAX_MSG_SIZE];

} message;

/* max messages value and message size */
const size_t MAXMSG = 9;
const size_t MSGSZ = sizeof(message);

#endif // STRUCTURES_H