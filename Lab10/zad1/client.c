#include "utils.h"

/* global variables */
int is_local;
char *nickname;
char *address;
int port;
int server_sock_id;
pthread_t thread_input;
char input[2];
int in_game = 0;
int is_empty[9];
char sign;
int positions[9] = {47,54,61,135,142,149,223,230,237};

/* auxiliary functions */
void handler();
void server_connect();
void server_disconnect();
void* read_input();
void get_user_input();
void update_empty(char *board);

/* program symbolizing client */
int main(int argc, char *argv[]) {
    if (argc < 4) {
        print_error("Not enough arguments!\nCorrect: ./client [Nickname] [local/network] [Unix Socket Path/IPv4 Address] [Port]?");
        return -1;
    }

    nickname = argv[1];
    is_local = (strcmp(argv[2],"local") == 0)? 1: 0;
    address = argv[3];

    if (!is_local && argc < 5) {
        print_error("Not enough arguments!\nCorrect: ./client [Nickname] [local/network] [Unix Socket Path/IPv4 Address] [Port]?");
        return -1;
    }

    if (!is_local)
        port = atoi(argv[4]);
    
    signal(SIGUSR1,handler);
    signal(SIGINT,server_disconnect);
    server_connect();
    
    for (int i=0; i<9; i++)
        is_empty[i] = 1;
    char buf[50];
    sprintf(buf,"%s %d",nickname,getpid());
    send_msg(server_sock_id,LOGIN_REQ,buf);
    message *msg = get_msg(server_sock_id);

    if (msg->mtype == LOGIN_SUC) {
        BLUE();
        printf("Successfully registered!\n");

        while (!0) {
            msg = get_msg(server_sock_id);

            if (msg->mtype == GAME_WAIT) {
                GREEN();
                printf("Waiting for another client to start game...\n");
            }
            else if (msg->mtype == GAME_FOUND) {
                RED();
                printf("Client found, game will start in a moment. Your sign is %s\n",msg->msg);
                in_game = 1;

                chess_game *g = (chess_game*)calloc(1,sizeof(chess_game));
                for (int i=0; i<9; i++)
                    g->map[i] = NOTHING;
                CYAN();
                char *map = get_square_map(g);
                printf("%s\n",map);
                sign = msg->msg[0];
                if (sign == 'X') {
                    get_user_input();
                    printf("Making move %s\n", input);
                    map[positions[atoi(input)-1]] = sign;
                    CYAN();
                    printf("Current board:\n%sOpponent is taking a turn\n",map);
                    send_msg(server_sock_id,GAME_MOVE,input);
                }
                else 
                    printf("Opponent is taking a turn\n");
                free(g);

                while(!0) {
                    msg = get_msg(server_sock_id);

                    if (msg->mtype == GAME_MOVE) {
                        CYAN();
                        printf("Opponent made a move. Current board:\n");
                        printf("%s",msg->msg);
                        update_empty(msg->msg);
                        get_user_input();
                        CYAN();
                        printf("Making move %s\n",input);
                        msg->msg[positions[atoi(input)-1]] = sign;
                        printf("Current board:\n%sOpponent is taking a turn\n",msg->msg);
                        send_msg(server_sock_id,GAME_MOVE,input);
                    }
                    else if (msg->mtype == GAME_END) {
                        BLUE();
                        in_game = 0;
                        printf("Game finished. %s\n",msg->msg);
                        send_msg(server_sock_id,LOGOUT,NULL);
                        printf("Logging out from the server...\n");
                        break;
                    }
                    else if (msg->mtype == PING) {
                        YELLOW();
                        send_msg(server_sock_id,PING,NULL);
                    }
                }
                break;
            }
            else if(msg->mtype == PING) {
                YELLOW();
                if (!in_game)
                    printf("Received PING\n");
                send_msg(server_sock_id,PING,NULL);
            }
        }
    }
    else {
        printf("Server rejected login. Reason: %s\n", msg->msg);
    }

    server_disconnect();

    return 0;
}

/* opening a connection */
void server_connect() {
    if (is_local) {
        struct sockaddr_un server_socket;
        server_socket.sun_family = AF_UNIX;
        strcpy(server_socket.sun_path,address);

        if ((server_sock_id = socket(AF_UNIX,SOCK_STREAM,0)) == -1) {
            print_error("Error while connecting to socket!");
            return;
        }
        if (connect(server_sock_id,(struct sockaddr*) &server_socket, sizeof(server_socket)) == -1) {
            print_error("Error while connecting to the socket!");
        }
    }
    else {
        struct sockaddr_in server_socket;
        server_socket.sin_family = AF_INET;
        server_socket.sin_addr.s_addr = inet_addr(address);
        server_socket.sin_port = htons(port);

        if ((server_sock_id = socket(AF_INET,SOCK_STREAM,0)) == -1) {
            print_error("Error while connecting to socket!");
            return;
        }
        if (connect(server_sock_id,(struct sockaddr*) &server_socket, sizeof(server_socket)) == -1) {
            print_error("Error while connecting to the socket!");
        }
    }
}

/* handler */
void handler() {
    printf("You've been disconnected due to other player disconnect\n");
    server_disconnect();
}

/* closing a connection */
void server_disconnect() {
    send_msg(server_sock_id,LOGOUT,NULL);

    if (shutdown(server_sock_id,SHUT_RDWR) == -1) {
        print_error("Error while shutdowning server socket!");
        return;
    }
    if (close(server_sock_id) == -1) {
        print_error("Error while closing server socket!");
    }
    BLUE();
    printf("\nDisconnected from the server!\n");
    exit(0);
}

/* function reading input */
void* read_input() {
    int empty_place = 0;
    while (!empty_place) {
        MAGENTA();
        printf("Make your move please [1-9]: ");
        scanf("%s",input);
        if (!is_empty[atoi(input)-1]) {
            printf("This place is already taken!\n");
        }
        else
            empty_place = 1;
    }
    pthread_exit((void *) 0);
}

/* function which reads user input */
void get_user_input() {
    input[0] = '.';
    if (pthread_create(&thread_input,NULL,read_input,NULL) == -1) {
        print_error("Error while reading input!");
        return;
    }
    message *msg;
    while (input[0] == '.') {
        msg = get_msg(server_sock_id);
        if (msg != NULL) {
            if (msg->mtype == PING) {
                YELLOW();
                if (!in_game)
                    printf("Received PING\n");
                send_msg(server_sock_id,PING,NULL);
            }
        }
    }
}

/* helping function which updates non-empty fields */
void update_empty(char *board) {
    for (int i=0; i<9; i++) {
        if (board[positions[i]] != ' ')
            is_empty[i] = 0;
    }
}