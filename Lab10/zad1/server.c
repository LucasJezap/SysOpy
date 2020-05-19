#include "utils.h"

/* global variables */
int un_sock_id;
int in_sock_id;
struct sockaddr_un un_socket;
struct sockaddr_in in_socket;

client* server_clients[MAX_NUMBER_OF_CLIENTS];
chess_game* games[MAX_NUMBER_OF_CLIENTS / 2];
int port;
char *socket_path;
int client_waiting = -1;

pthread_t game_thread;
pthread_t ping_thread;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* auxiliary functions */
void open_server();
void shut_connection(int id, int i);
int add_client(int id, char* nickname, char *pid);
void delete_client(int id);
int do_login(int sock_id);
int new_game(int player1_id, int player2_id);
void close_game(int i);
void create_match(int r_index);
void close_server();
void* game_thread_function();
void* ping_thread_function();

/* program symbolizing server */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_error("Wrong number of arguments!\nCorrect: ./server [TCP Port] [Unix Socket Path]");
        return -1;
    }
    
    srand(time(NULL));
    port = atoi(argv[1]);
    socket_path = argv[2];
    signal(SIGINT,close_server);
    
    open_server();

    if (pthread_create(&game_thread,NULL,game_thread_function,NULL) < 0) {
        print_error("Error while creating game thread!");
        return -1;
    }
    if (pthread_create(&ping_thread,NULL,ping_thread_function,NULL) < 0) {
        print_error("Error while creating ping thread!");
        return -1;
    }
    if (pthread_join(game_thread,NULL) < 0) {
        print_error("Error while joining game thread!");
        return -1;
    }
    if (pthread_join(ping_thread,NULL) < 0) {
        print_error("Error while joining ping thread!");
        return -1;
    }

    close_server();

    return 0;
}

/* function opening the server */
void open_server() {
    /* local */
    un_socket.sun_family = AF_UNIX;
    strcpy(un_socket.sun_path,socket_path);

    if ((un_sock_id = socket(AF_UNIX,SOCK_STREAM,0)) == -1) {
        print_error("Error while getting socket id!");
        return;
    }

    if (bind(un_sock_id,(struct sockaddr*) &un_socket, sizeof(un_socket)) == -1) {
        print_error("Error while binding socket!");
        return;
    }

    if (listen(un_sock_id,MAX_NUMBER_OF_CLIENTS) == -1) {
        print_error("Error while listen function!");
        return;
    }
    
    BLUE();
    printf("Server is starting...\n");
    printf("Local/Unix socket path: %s\n",socket_path);

    /* network */
    struct hostent* host = gethostbyname("localhost");
    struct in_addr host_addr = *(struct in_addr*) host->h_addr;
    in_socket.sin_family = AF_INET;
    in_socket.sin_addr.s_addr = host_addr.s_addr;
    in_socket.sin_port = htons(port);

    if ((in_sock_id = socket(AF_INET,SOCK_STREAM,0)) == -1) {
        print_error("Error while getting socket id!");
        return;
    }

    if (bind(in_sock_id,(struct sockaddr*) &in_socket, sizeof(in_socket)) == -1) {
        print_error("Error while binding socket!");
        return;
    }

    if (listen(in_sock_id,MAX_NUMBER_OF_CLIENTS) == -1) {
        print_error("Error while listen function!");
        return;
    }

    BLUE();
    printf("Network socket path: %s, port: %d\n", inet_ntoa(host_addr), port);
}

/* closing connection with client */
void shut_connection(int id, int i) {
    if (i == client_waiting)
        client_waiting = -1;
    else {
        chess_game *game = games[server_clients[i]->current_game];
        int second = game->player1_id == i? game->player2_id: game->player1_id;
        if (server_clients[second] != NULL) {
            RED();
            printf("The  Client %d will be disconnected due to Client %d logout\n",second,i);
            kill(server_clients[second]->client_pid,SIGUSR1);
        }
    }
    if (shutdown(id,SHUT_RDWR) == -1) {
        print_error("Error while shutdowning connection with client!");
        return;
    }
    if (close(id) == -1) {
        print_error("Error while closing connection with client!");
    }
}

/* adding new client */
int add_client(int id, char* nickname, char *pid) {
    int index = -1;
    for (int i=0; i<MAX_NUMBER_OF_CLIENTS; i++) {
        if (server_clients[i] != NULL && strcmp(nickname,server_clients[i]->nickname) == 0)
            return -1;
        if (server_clients[i] == NULL && index == -1) {
            index = i;
            break;
        }
    }
    if (index == -1) return -1;
    server_clients[index] = create_client(id,nickname, pid);
    return index;
}

/* deleting client */
void delete_client(int id) {
    for (int i=0; i<MAX_NUMBER_OF_CLIENTS; i++) {
        if (server_clients[i] != NULL && server_clients[i]->id == id) {
            RED();
            printf("Client %d has been disconnected!\n",i);
            server_clients[i] = NULL;
            break;
        }
    }
}

/* login */
int do_login(int sock_id) {
    RED();
    printf("New login attempt...\n");

    int new_client_sock;
    if ((new_client_sock = accept(sock_id,NULL,NULL)) == -1) {
        print_error("Error while awaiting a connection on socket!");
        return -1;
    }

    message *msg = get_msg(new_client_sock);
    char *nickname = strtok(msg->msg," ");
    char *pid = strtok(NULL,"\n");
    printf("Nickname: %s\n", nickname);

    int r_index = add_client(new_client_sock,msg->msg,pid);
    if (r_index == -1) {
        printf("Login unsuccessful...\n");
        send_msg(new_client_sock,LOGIN_FAIL,"login already exists");
    }
    else {
        printf("Login successful...\n");
        send_msg(new_client_sock,LOGIN_SUC,NULL);
    }
    return r_index;
}

/* creating a new game */
int new_game(int player1_id, int player2_id) {
    for (int i=0; i<MAX_NUMBER_OF_CLIENTS/2; i++) {
        if (games[i] == NULL) {
            games[i] = start_new_game(player1_id,player2_id);
            return i;
        }
    }
    return -1;
}

/* ending a game */
void close_game(int i) {
    games[i] = NULL;    
}

/* creating a new match */
void create_match(int r_index) {
    CYAN();
    if (client_waiting < 0) {
        printf("There is no waiting client!\n");
        send_msg(server_clients[r_index]->id,GAME_WAIT,NULL);
        client_waiting = r_index;
    }
    else {
        printf("There's a waiting Client %d\n",client_waiting);
        int g_index = new_game(r_index,client_waiting);
        server_clients[r_index]->current_game = server_clients[client_waiting]->current_game = g_index;
        if (rand()%2 == 0) {
            send_msg(server_clients[r_index]->id,GAME_FOUND,"O");
            send_msg(server_clients[client_waiting]->id,GAME_FOUND,"X");
            server_clients[r_index]->current_sign = O;
            server_clients[client_waiting]->current_sign = X;
        }
        else {
            send_msg(server_clients[r_index]->id,GAME_FOUND,"X");
            send_msg(server_clients[client_waiting]->id,GAME_FOUND,"O");
            server_clients[r_index]->current_sign = X;
            server_clients[client_waiting]->current_sign = O;
        }
        client_waiting = -1;
    }
}

/* function for processing clients and connecting them */
void* game_thread_function() {
    struct pollfd ids[MAX_NUMBER_OF_CLIENTS+2];

    ids[MAX_NUMBER_OF_CLIENTS].fd = un_sock_id;
    ids[MAX_NUMBER_OF_CLIENTS].events = POLLIN;
    ids[MAX_NUMBER_OF_CLIENTS + 1].fd = in_sock_id;
    ids[MAX_NUMBER_OF_CLIENTS + 1].events = POLLIN;
    
    while(!0) {
        pthread_mutex_lock(&mutex);
        for (int i=0; i<MAX_NUMBER_OF_CLIENTS; i++) {
            ids[i].fd = (server_clients[i] == NULL)? -1: server_clients[i]->id;
            ids[i].events = POLLIN;
            ids[i].revents = 0;
        }
        pthread_mutex_unlock(&mutex);

        ids[MAX_NUMBER_OF_CLIENTS].revents = ids[MAX_NUMBER_OF_CLIENTS+1].revents = 0;
        GREEN();
        printf("Waiting for clients (polling) ...\n");

        poll(ids,MAX_NUMBER_OF_CLIENTS+2,-1);

        pthread_mutex_lock(&mutex);
        MAGENTA();
        for (int i=0; i<MAX_NUMBER_OF_CLIENTS+2; i++) {
            if (i < MAX_NUMBER_OF_CLIENTS && server_clients[i] == NULL)
                continue;
            
            if (ids[i].revents & POLLHUP) {
                shut_connection(ids[i].fd,i);
                delete_client(ids[i].fd);
            }
            else if (ids[i].revents & POLLIN) {
                if (ids[i].fd == un_sock_id || ids[i].fd == in_sock_id) {
                    int r_index = do_login(ids[i].fd);
                    printf("New Client registered at index %d\n",r_index);
                    if (r_index >= 0) create_match(r_index);
                }
                else {
                    printf("Received message from the Client %d\n",i);
                    message *msg = get_msg(ids[i].fd);
                    if (msg->mtype == GAME_MOVE) {
                        printf("Move made: %s\n", msg->msg);
                        chess_game *game = games[server_clients[i]->current_game];
                        int field = atoi(msg->msg)-1;
                        square sign = server_clients[i]->current_sign;
                        game->map[field] = sign;
                        int second = game->player1_id == i? game->player2_id: game->player1_id;

                        if (check_game_result(game) == UNCOMPLETED) {
                            send_msg(ids[second].fd,GAME_MOVE,get_square_map(game));
                        }
                        else {
                            int is_O_winner = (check_game_result(game) == O_W)? 1: 0;
                            RED();
                            if (server_clients[i]->current_sign == O && is_O_winner) printf("Client %d has won the game!\n",i);
                            if (server_clients[i]->current_sign == O && !is_O_winner) printf("Client %d has won the game!\n",second);
                            if (server_clients[i]->current_sign == X && is_O_winner) printf("Client %d has won the game!\n",second);
                            if (server_clients[i]->current_sign == X && !is_O_winner) printf("Client %d has won the game!\n",i);
                            if (is_O_winner) {
                                char *first_msg = (server_clients[i]->current_sign == O)? "You're a winner!": "You're a loser!";
                                char *second_msg = (server_clients[second]->current_sign == O)? "You're a winner!": "You're a loser!";
                                send_msg(ids[i].fd,GAME_END,first_msg);
                                send_msg(ids[second].fd,GAME_END,second_msg);
                            }
                            else {
                                char *first_msg = (server_clients[i]->current_sign == X)? "You're a winner!": "You're a loser!";
                                char *second_msg = (server_clients[second]->current_sign == X)? "You're a winner!": "You're a loser!";
                                send_msg(ids[i].fd,GAME_END,first_msg);
                                send_msg(ids[second].fd,GAME_END,second_msg);
                            }
                        }
                    }
                    else if (msg->mtype == PING) {
                        server_clients[i]->is_responding = 1;
                    }
                    else if (msg->mtype == LOGOUT) {
                        shut_connection(ids[i].fd,i);
                        delete_client(ids[i].fd);
                    }
                }
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit((void *) 0);
}

/* function for processing pings */
void* ping_thread_function() {
    while(!0) {
        sleep(PING_TIME);
        YELLOW();
        printf("Server is pinging it's clients...\n");

        pthread_mutex_lock(&mutex);
        YELLOW();
        for (int i=0; i<MAX_NUMBER_OF_CLIENTS; i++) {
            if (server_clients[i] != NULL) {
                printf("Pinging Client %d...\n",i);
                server_clients[i]->is_responding = 0;
                send_msg(server_clients[i]->id,PING,NULL);
            }
        }

        pthread_mutex_unlock(&mutex);
        YELLOW();
        printf("Server is now waiting for responses of all clients...\n");

        sleep(PING_RETURN_TIME);
        pthread_mutex_lock(&mutex);
        YELLOW();
        for (int i=0; i<MAX_NUMBER_OF_CLIENTS; i++) {
            if (server_clients[i] != NULL && server_clients[i]->is_responding == 0) {
                printf("Client %d didn't respond to ping, he will be disconnected...\n",i);
                shut_connection(server_clients[i]->id,i);
                delete_client(server_clients[i]->id);
            }
        }

        pthread_mutex_unlock(&mutex);
    }
    pthread_exit((void *) 0);
}

/* Function closing the server */
void close_server() {
    for (int i=0; i<MAX_NUMBER_OF_CLIENTS; i++) {
        if (server_clients[i] != NULL) {
            kill(server_clients[i]->client_pid,SIGINT);
        }
    }

    if (pthread_cancel(game_thread) == -1) {
        print_error("Error while cancelling game thread!");
        return;
    }
    if (pthread_cancel(ping_thread) == -1) {
        print_error("Error while cancelling ping thread!");
        return;
    }
    if (shutdown(un_sock_id,SHUT_RDWR) == -1) {
        print_error("Error while shutdowning Unix Socket!");
        return;
    }
    if (close(un_sock_id) == -1) {
        print_error("Error while closing Unix Socket!");
        return;
    }
    if (unlink(socket_path) == -1) {
        print_error("Error while unlinking socket path!");
        return;
    }
    if (shutdown(in_sock_id,SHUT_RDWR) == -1) {
        print_error("Error while shutdowning Network Socket!");
        return;
    }
    if (close(in_sock_id) == -1) {
        print_error("Error while closing Network Socket!");
    }
    BLUE();
    printf("\nServer successfully closed!\n");
    exit(0);
}