/* own helpful header file */
#include "structures.h"

/* global variables */
int turn;
int client_id;
int pair_id;
int pair_queue_id;
int in_chat = 0;
int queue_id;
int server_queue_id;
int key;
int running = 1;


/* exit function closing the client */
void close_client() {
    running = 0;
    /* checking if queue is still open */
    if (queue_id != -1) {
        if (msgctl(queue_id,IPC_RMID,NULL) == -1)
            printf("Error while closing queue!\n");
        else {
            printf("\nQueue deleted successfully and client is about to close\n");
        }
    }
}

/* getting the list of all users */
void process_list() {
    /* creating message to the server */
    message *msg = (message *)calloc(1,sizeof(message));
    msg->mtype = LIST;
    msg->client_id = client_id;

    /* sending message to the server */
    if (msgsnd(server_queue_id,msg,MSGSZ,0) == -1) {
        ERROR("Error while sending message to the server!\n");
    }

    sleep(1);
    
    /* receiving back message from the server */ 
    message *msg2 = (message *)calloc(1,sizeof(message));
    msgrcv(queue_id,msg2,MSGSZ,4,0);
    
    /* printing the message */
    printf("%s",msg2->message);

    free(msg);
    free(msg2);
}

void process_connect() {
    /* getting the user client wants to connect */
    printf("What's the id of the client you want to connect?\n");
    int user2_id;
    scanf("%d",&user2_id);

    /* creating message to the server */
    message *msg = (message *)calloc(1,sizeof(message));
    msg->mtype = CONNECT;
    msg->client_id = client_id;
    msg->pair_id = user2_id;

    /* sending message to the server */
    if (msgsnd(server_queue_id,msg,MSGSZ,0) == -1) {
        ERROR("Error while sending message to the server!\n");
    }

    sleep(1);
    
    /* receiving back message from the server */ 
    message *msg2 = (message *)calloc(1,sizeof(message));
    msgrcv(queue_id,msg2,MSGSZ,5,0);
    
    /* printing the message */
    printf("%s",msg2->message);

    /* setting connected client id and his queue */
    pair_id = user2_id;
    pair_queue_id = msg2->queue_id;
    in_chat = 1;
    turn = 1;

    free(msg);
    free(msg2);
}

void process_disconnect() {
    /* creating message to the server */
    message *msg = (message *)calloc(1,sizeof(message));
    msg->mtype = DISCONNECT;
    msg->client_id = client_id;
    msg->pair_id = pair_id;

    /* sending message to the server */
    if (msgsnd(server_queue_id,msg,MSGSZ,0) == -1) {
        ERROR("Error while sending message to the server!\n");
    }

    sleep(1);
    
    /* receiving back message from the server */ 
    message *msg2 = (message *)calloc(1,sizeof(message));
    msgrcv(queue_id,msg2,MSGSZ,3,0);
    
    /* printing the message */
    printf("\n%s",msg2->message);

    free(msg);
    free(msg2);
    in_chat = 0;
}

void process_stop() {
    /* creating message to the server */
    message *msg = (message *)calloc(1,sizeof(message));
    msg->mtype = STOP;
    msg->client_id = client_id;

    /* sending message to the server */
    if (msgsnd(server_queue_id,msg,MSGSZ,0) == -1) {
        ERROR("Error while sending message to the server!\n");
    }
    
    /* receiving back message from the server */ 
    message *msg2 = (message *)calloc(1,sizeof(message));
    msgrcv(queue_id,msg2,MSGSZ,2,0);
    
    /* printing the message */
    printf("\n%s",msg2->message);

    free(msg);
    free(msg2);

    /* exiting */
    exit(0);
}

/* chatting */
void chat() {
    printf("\n\n----------CHAT MODE----------\nWrite DISCONNECT to exit\n");
    size_t m_size = 200;
    char buffer[m_size];
    
    while (1) {
        /* write turn */
        if (turn == 1) {
            /* preparing message */
            printf("Client%d: ",client_id);
            fgets(buffer,m_size,stdin);
            message *msg = (message *)calloc(1,sizeof(message));
            msg->mtype = CHAT;

            /* copying appropriate message */
            if (strcmp(buffer,"\n") == 0){
                printf("Hello Client%d\n",pair_id);
                sprintf(msg->message,"Hello Client%d\n",pair_id);
            }
            else if (strcmp(buffer,"DISCONNECT\n") != 0)
                strcpy(msg->message,buffer);
            else
                strcpy(msg->message,"Has disconnected from the chat\n");

            /* send message */
            if (msgsnd(pair_queue_id,msg,MSGSZ,0) == -1) 
                ERROR("Error while sending message to the client!\n");

            /* end chat */
            if (strcmp(buffer,"DISCONNECT\n") == 0) {
                process_disconnect();
                break;
            }

            free(msg);
                    
            turn = 0;
        }
        /* receive turn */
        else {
            /* prepare data */
            struct msqid_ds state;
            message rcv;

            /* wait for message */
            while(1) {
                if(msgctl(queue_id,IPC_STAT, &state) == -1)
                    ERROR("Error while reading current state!\n");

                if (state.msg_qnum > 0) {
                    msgrcv(queue_id,&rcv,MSGSZ,6,0);
                    printf("Client%d: %s",pair_id,rcv.message);
                    break;
                }
            }   
            turn = 1;
        }
    }
    printf("\n\n----------EXITING CHAT----------\n");
    in_chat = 0;
}

/* function waiting for and processing client's input */
void process_input() {
    /* going to chat mode */
    if (in_chat)
        chat();

    /* getting user input */
    char buffer[100];
    scanf("%s",buffer);

    /* process input */
    if (strcmp(buffer,"LIST") == 0) {
        process_list();
    }
    else if (strcmp(buffer,"CONNECT") == 0) {
        process_connect();
    }
    else if (strcmp(buffer,"DISCONNECT") == 0) {
        process_disconnect();
    }
    else if (strcmp(buffer,"STOP") == 0) {
        process_stop();
    }
    else if (in_chat) {
        printf("wow im in chat\n");
    }
    else {
        printf("Wrong input!\n");
    }
}

/* handler for SIGINT */
void handler(int signum) {
    process_stop();
}

/* handler for SIGUSR1 -> it connects with the user who tried to connect with us */
void usr1_handler(int signum) {
    /* receiving back message from the server */ 
    message *msg = (message *)calloc(1,sizeof(message));
    msgrcv(queue_id,msg,MSGSZ,5,0);
            
    /* printing the message */
    printf("%s",msg->message);

    /* setting connected client id and his queue */
    pair_id = msg->client_id;
    pair_queue_id = msg->queue_id;
    in_chat = 1;
    turn = 0;

    sleep(1);

    /* putting signal handler back */
    if (signal(SIGUSR1, usr1_handler) == SIG_ERR)
        ERROR("Error while establishing handler for SIGUSR1!\n");
    
    process_input();
}

/* helping menu */
void print_menu() {
    printf("Hello!\nHere's a list of possible actions:\nLIST -> get list of all users\nCONNECT [client_id] -> connect with client "
            "(first choose only CONNECT, you provide id later)\nDISCONNECT -> disconnect from current chat\nSTOP -> disconnect from the server\n");
}

/* client */
int main() {
    /* setting handlers */
    if (signal(SIGINT, handler) == SIG_ERR) 
        ERROR("Error while establishing handler for SIGINT!\n");
    
    if (signal(SIGUSR1, usr1_handler) == SIG_ERR)
        ERROR("Error while establishing handler for SIGUSR1!\n");
    
    /* setting exit function */
    if (atexit(close_client) == -1)
        ERROR("Error while setting exit function with atexit!\n");

    /* setting server queue_id */
    server_queue_id = msgget(ftok(PATH,PROJ),0);
    if (server_queue_id == -1)
        ERROR("Error while accessing server queue!\n");

    /* setting key based on HOME folder */
    key = ftok(PATH,getpid());
    if (key == -1) 
        ERROR("Error while generating key for queue!\n");

    /* creating new queue */
    queue_id = msgget(key,0666 | IPC_CREAT | IPC_EXCL);
    if (queue_id == -1)
        ERROR("Error while creating new queue!\n");

    /* creating init message and sending it to server */
    message *msg = (message *)calloc(1,sizeof(message));
    msg->mtype = INIT;
    msg->queue_key = key;
    msg->queue_id = queue_id;
    msg->client_pid = getpid();

    if (msgsnd(server_queue_id,msg,MSGSZ,0) == -1) {
        ERROR("Error while sending message to the server!\n");
    }    
    free(msg);

    sleep(1);

    /* receiving return message from server with client's unique id */
    message *msg2 = (message *)calloc(1,sizeof(message));
    msgrcv(queue_id,msg2,MSGSZ,1,0);
    client_id = msg2->client_id;

    free(msg2);

    print_menu();
    /* client in action */
    while (!0) {
        if (running) {
            process_input();
        }
        else 
            break;
    }

    /* executing exit function -> close_server() */
    return 0;
}