/* own helpful header file */
#include "structures.h"

/* global variables */
int queue_id;
int key;
int number_of_clients = 0;
int running = 1;
client clients[CLIENT_LIMIT];

/* user initialization */
void process_init(message *rcv) {
    /* setting client's queue key */
    clients[number_of_clients].queue_key = rcv->queue_key;

    /* setting client's id */
    clients[number_of_clients].client_id = number_of_clients;

    /* setting client's own queue */
    clients[number_of_clients].queue_id = rcv->queue_id;
    
    /* bonus info */
    clients[number_of_clients].active = 1;
    clients[number_of_clients].disconnected = 0;
    clients[number_of_clients].client_pid = rcv->client_pid;

    /* incrementing number of active clients */
    number_of_clients++;

    /* creating message to client */
    message *to_client = (message *)calloc(1,sizeof(message));
    to_client->mtype = INIT;
    to_client->client_id = number_of_clients-1;
    strcpy(to_client->message,"You have been successfully connected to the server!\n");

    /* sending back information to client */
    if (msgsnd(clients[number_of_clients-1].queue_id,to_client,MSGSZ,0) == -1)
        ERROR("Error while sending message to the client!\n");
    
    free(to_client);

    /* info to the server */
    printf("New client with id = %d has connected to the server\n",number_of_clients-1);
}

/* client disconnects from the server */
void process_stop(message *rcv) {
    /* disabling flag */
    clients[rcv->client_id].disconnected = 1;

    /* creating message to client */
    message *to_client = (message *)calloc(1,sizeof(message));
    to_client->mtype = STOP;
    to_client->client_id = rcv->client_id;
    strcpy(to_client->message,"You have been successfully disconnected from the server!\n");

    /* sending back information to client */
    if (msgsnd(clients[rcv->client_id].queue_id,to_client,MSGSZ,0) == -1)
        ERROR("Error while sending message to the client!\n");

    free(to_client);

    /* info to the server */
    printf("Client with id = %d has disconnected from the server\n",rcv->client_id);
}

/* disconnecting user from current chat */
void process_disconnect(message *rcv) {
    /* disabling flag */
    clients[rcv->client_id].active = 1;

    /* creating message to client */
    message *to_client = (message *)calloc(1,sizeof(message));
    to_client->mtype = DISCONNECT;
    to_client->client_id = rcv->client_id;
    strcpy(to_client->message,"You have been successfully disconnected from the server!\n");

    /* sending back information to client */
    if (msgsnd(clients[rcv->client_id].queue_id,to_client,MSGSZ,0) == -1)
        ERROR("Error while sending message to the client!\n");

    free(to_client);

    /* info to the server */
    printf("Client with id = %d has disconnected from the chat with client with id = %d\n",rcv->client_id,rcv->pair_id);
}

/* print list of active users */
void process_list(message *rcv) {
    /* creating message to client */
    message *to_client = (message *)calloc(1,sizeof(message));
    to_client->mtype = LIST;

    for (int i=0; i<number_of_clients; i++) {
        sprintf(to_client->message + strlen(to_client->message),"Client with ID = %d ",clients[i].client_id);
        if (clients[i].active) 
            sprintf(to_client->message + strlen(to_client->message),"is active\n");
        else 
            sprintf(to_client->message + strlen(to_client->message),"is unactive\n");
    }

    printf("%d\n",rcv->client_id);
    /* sending back information to client */
    if (msgsnd(clients[rcv->client_id].queue_id,to_client,MSGSZ,0) == -1)
        ERROR("Error while sending message to the client!\n");

    free(to_client);

    /* info to the server */
    printf("Client with id = %d has asked to see the list of all online users\n",rcv->client_id);
}

/* connect user to another user */
void process_connect(message *rcv) {
    /* finding user2 queue_id */
    int queue_id_to_find = -1;
    pid_t user2_pid;

    for (int i=0; i<number_of_clients; i++) {
        if (clients[i].active && clients[i].client_id == rcv->pair_id) {
            user2_pid = clients[i].client_pid;
            queue_id_to_find = clients[i].queue_id;
        }
    }

    /* creating message to client */
    message *to_client = (message *)calloc(1,sizeof(message));
    to_client->mtype = CONNECT;
    to_client->client_id = rcv->pair_id;
    to_client->queue_id = queue_id_to_find;
    if (queue_id_to_find != -1)
        sprintf(to_client->message,"You have been successfully connected to client with ID = %d\n",rcv->pair_id);

    /* sending back information to client */
    if (msgsnd(clients[rcv->client_id].queue_id,to_client,MSGSZ,0) == -1)
        ERROR("Error while sending message to the client!\n");

    free(to_client);
    /* if it was a wrong call we don't send info to user2 */
    if (queue_id_to_find == -1)
        return;

    queue_id_to_find = -1;

    /* finding user1 queue_id */
    for (int i=0; i<number_of_clients; i++) {
        if (clients[i].active && clients[i].client_id == rcv->client_id) {
            queue_id_to_find = clients[i].queue_id;
        }
    }

    /* creating message to client */
    message *to_client2 = (message *)calloc(1,sizeof(message));
    to_client2->mtype = CONNECT;
    to_client2->client_id = rcv->client_id;
    to_client2->queue_id = queue_id_to_find;
    if (queue_id_to_find != -1)
        sprintf(to_client2->message,"You have been successfully connected to client with ID = %d\n",rcv->client_id);

    /* sending back information to client */
    if (msgsnd(clients[rcv->pair_id].queue_id,to_client2,MSGSZ,0) == -1)
        ERROR("Error while sending message to the client!\n");

    /* sending signal to user2 */
    kill(user2_pid,SIGUSR1);

    free(to_client2);

    /* making users unactive */
    clients[rcv->client_id].active = 0;
    clients[rcv->pair_id].active = 0;

    /* info to the server */
    printf("Client with id = %d has connected to the chat with client with id = %d\n",rcv->client_id,rcv->pair_id);
}

/* processing received message based on type */
void process_msg(message *rcv) {
    if (rcv != NULL) {
        switch(rcv->mtype) {
            /* user initialization */
            case INIT: {
                process_init(rcv);
                break;
            }
            /* server stopping */
            case STOP: {
                process_stop(rcv);
                break;
            }
            /* disconnecting user from current chat */
            case DISCONNECT: {
                process_disconnect(rcv);
                break;
            }
            /* print list of active users */
            case LIST: {
                process_list(rcv);
                break;
            }
            /* connect user to another user */
            case CONNECT: {
                process_connect(rcv);
                break;
            }
            default: {
                break;
            }
        }
    }
}

/* closing all clients queues and sending them info */
void close_clients() {
    /* closing active users */
    printf("\n");
    for (int i=0; i<number_of_clients; i++) {
        if (!clients[i].disconnected) {
            /* disconnecting from the list */
            clients[i].disconnected = 1;

            /* sending signal */
            kill(clients[i].client_pid,SIGINT);
            message rcv;
            msgrcv(queue_id,&rcv,MSGSZ,2,0);
            process_msg(&rcv);
        }
    }
}

/* exit function closing the server */
void close_server() {
    /* closing all clients queues and sending them info */
    close_clients();
    sleep(1);
    /* checking if queue is still open */
    if (queue_id != -1) {
        if (msgctl(queue_id,IPC_RMID,NULL) == -1)
            printf("Error while closing queue!\n");
        else {
            printf("\nQueue deleted successfully and server is about to close\n");
        }
    }
}

/* handler for SIGINT */
void handler(int signum) {
    running = 0;
}

/* server */
int main() {
    /* setting handler */
    if (signal(SIGINT, handler) == SIG_ERR) 
        ERROR("Error while establishing handler for SIGINT!\n");
    
    /* setting exit function */
    if (atexit(close_server) == -1)
        ERROR("Error while setting exit function with atexit!\n");

    /* setting key based on HOME folder*/
    key = ftok(PATH,PROJ);
    if (key == -1) 
        ERROR("Error while generating key for queue!\n");

    /* creating new queue */
    queue_id = msgget(key,0666 | IPC_CREAT | IPC_EXCL);
    if (queue_id == -1)
        ERROR("Error while creating new queue!\n");

    /* server in action */
    printf("The server is running...\n");
    message rcv;
    struct msqid_ds state;
    while (!0) {
        if (running) {
            /* getting message with lowest type and processing it */
            if(msgctl(queue_id,IPC_STAT, &state) == -1)
                ERROR("Error while reading current state!\n");
            if (state.msg_qnum > 0) {
                msgrcv(queue_id,&rcv,MSGSZ,-6,0);
                process_msg(&rcv);
            }
        }
        else 
            break;
    }

    /* executing exit function -> close_server() */
    return 0;
}