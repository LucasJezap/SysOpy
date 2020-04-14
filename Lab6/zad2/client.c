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
int server_pid;
char path[100];
int running = 1;


/* exit function closing the client */
void close_client() {
    running = 0;
    /* checking if queue is still open */
    if (queue_id != -1) {
        /* creating message to the server */
        message *msg = (message *)calloc(1,sizeof(message));
        msg->mtype = STOP;
        msg->client_id = client_id;

        /* sending message to the server */
        if (mq_send(server_queue_id,(char *) msg,MSGSZ,5) == -1) {
            ERROR("Error while sending message to the server!\n");
        }

        /* informing server about the message */
        kill(server_pid,SIGUSR1);

        usleep(250000);
        
        /* closing server's queue */
        if (mq_close(server_queue_id) == -1)
            printf("\nError while closing server queue!\n");
        else {
            printf("\nServer queue closed successfully \n");
        }

        /* closing and deleting client's queue */
        if (mq_close(queue_id) == -1 || mq_unlink(path) == -1)
            printf("Error while closing queue!\n");
        else {
            printf("Queue deleted successfully and client is about to close\n");
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
    if (mq_send(server_queue_id,(char *) msg,MSGSZ,3) == -1) {
        ERROR("Error while sending message to the server!\n");
    }

    sleep(1);
    
    /* receiving back message from the server */ 
    message *msg2 = (message *)calloc(1,sizeof(message));
    mq_receive(queue_id,(char *) msg2,MSGSZ,NULL);
    
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
    msg->client_pid = getpid();

    /* sending message to the server */
    if (mq_send(server_queue_id,(char *) msg,MSGSZ,2) == -1) {
        ERROR("Error while sending message to the server!\n");
    }

    sleep(1);
    
    /* receiving back message from the server */ 
    message *msg2 = (message *)calloc(1,sizeof(message));
    mq_receive(queue_id,(char *) msg2,MSGSZ,NULL);
    
    /* printing the message */
    printf("%s",msg2->message);

    /* setting connected client id and his queue */
    pair_id = user2_id;
    char pth[20];
    sprintf(pth,"/%d",msg2->client_pid);
    pair_queue_id = mq_open(pth,O_WRONLY);
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
    if (mq_send(server_queue_id,(char *) msg,MSGSZ,4) == -1) {
        ERROR("Error while sending message to the server!\n");
    }

    sleep(1);
    
    /* receiving back message from the server */ 
    message *msg2 = (message *)calloc(1,sizeof(message));
    mq_receive(queue_id,(char *) msg2,MSGSZ,NULL);
    
    /* printing the message */
    printf("\n%s",msg2->message);

    free(msg);
    free(msg2);
    in_chat = 0;
}

void process_stop() {
    printf("LOLOLO");
    /* creating message to the server */
    message *msg = (message *)calloc(1,sizeof(message));
    msg->mtype = STOP;
    msg->client_id = client_id;
    /* sending message to the server */
    if (mq_send(server_queue_id,(char *) msg,MSGSZ,5) == -1) {
        ERROR("Error while sending message to the server!\n");
    }
    
    /* receiving back message from the server */ 
    message *msg2 = (message *)calloc(1,sizeof(message));
    while(1) {
        mq_receive(queue_id,(char *) msg2,MSGSZ,NULL);
        if (msg2->mtype == STOP) break;
    }
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
            if (mq_send(pair_queue_id,(char *) msg,MSGSZ,1) == -1)
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
            struct mq_attr state;
            message rcv;

            /* wait for message */
            while(1) {
                if(mq_getattr(queue_id,&state) == -1)
                    ERROR("Error while reading current state!\n");

                if (state.mq_curmsgs > 0) {
                    mq_receive(queue_id,(char *) &rcv,MSGSZ,NULL);
                    printf("Client%d: %s",pair_id,rcv.message);
                    break;
                }
            }   
            turn = 1;
        }
    }
    printf("\n\n----------EXITING CHAT----------\n");
    in_chat = 0;
    mq_close(pair_queue_id);
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
    mq_receive(queue_id,(char *) msg,MSGSZ,NULL);
            
    /* printing the message */
    printf("%s",msg->message);

    /* setting connected client id and his queue */
    pair_id = msg->client_id;
    char pth[20];
    sprintf(pth,"/%d",msg->client_pid);
    pair_queue_id = mq_open(pth,O_WRONLY);
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

    /* creating new queue for path made from pid */
    sprintf(path,"/%d",getpid());

    struct mq_attr attr;
    attr.mq_maxmsg = MAXMSG;
    attr.mq_msgsize = MSGSZ;

    queue_id = mq_open(path,O_RDONLY | O_CREAT | O_EXCL, 0666,&attr);
    if (queue_id == -1)
        ERROR("Error while creating new queue!\n");

    /* setting server queue_id */
    server_queue_id = mq_open(PATH,O_WRONLY);
    if (server_queue_id == -1)
        ERROR("Error while accessing server queue!\n");

    /* creating init message and sending it to server */
    message *msg = (message *)calloc(1,sizeof(message));
    msg->mtype = INIT;
    strcpy(msg->path,path);
    msg->client_pid = getpid();

    if (mq_send(server_queue_id,(char *) msg,MSGSZ,6) == -1) {
        ERROR("Error while sending message to the server!\n");
    }    
    free(msg);

    sleep(1);

    /* receiving return message from server with client's unique id */
    message *msg2 = (message *)calloc(1,sizeof(message));
    mq_receive(queue_id,(char *) msg2,MSGSZ,NULL);
    client_id = msg2->client_id;
    server_pid = msg2->client_pid;

    free(msg2);

    print_menu();
    printf("You've been granted ClientID = %d\n",client_id);
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