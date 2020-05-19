#ifndef UTILS_H
#define UTILS_H

#define _GNU_SOURCE

/* bunch of includes, meh */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/un.h>
#include <signal.h>
#include <pthread.h>
#include <poll.h>
#include <time.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <endian.h>

/* colors */
#define RED() (printf("\033[1;31m"))
#define YELLOW() (printf("\033[1;33m"))
#define GREEN() (printf("\033[1;32m"))
#define BLUE() (printf("\033[1;34m"))
#define MAGENTA() (printf("\033[1;35m"))
#define CYAN() (printf("\033[1;36m"))
#define DEFAULT() (printf("\033[0m"))

/* defined constants */
#define MAX_NUMBER_OF_CLIENTS 10
#define MAX_LENGTH_OF_MSG 300
#define PING_TIME 3
#define PING_RETURN_TIME 2

/* type of message */
typedef enum message_type {
    LOGIN_REQ,
    LOGIN_SUC,
    LOGIN_FAIL,
    GAME_WAIT,
    GAME_FOUND,
    GAME_MOVE,
    GAME_END,
    PING,
    LOGOUT
} message_type;

/* message struct */
typedef struct message {
    message_type mtype;
    char msg[MAX_LENGTH_OF_MSG-1]; // '\0'
} message;

/* chess square enum */
typedef enum square {
    O,
    X,
    NOTHING
} square;

/* chess game struct */
typedef struct chess_game {
    int player1_id;
    int player2_id;
    square map[9];
} chess_game;

/* chess game results enum */
typedef enum game_result {
    O_W,
    X_W,
    UNCOMPLETED,
    DRAW
} game_result;

/* client struct */
typedef struct client {
    int id;
    char nickname[MAX_LENGTH_OF_MSG]; // cant have larger size
    int is_responding;
    int current_game;
    square current_sign;
    pid_t client_pid;
} client;

/* handling errors */
void print_error(char *error);

/* start new game */
chess_game* start_new_game(int player1_id, int player2_id);

/* make a move in game */
void chess_move(chess_game *game, square sign, int square_no);

/* get a 3x3 board */
char* get_square_map(chess_game *game);

/* check if game has already ended */
game_result check_game_result(chess_game *game);

/* creating a new client */
client* create_client(int id, char *nickname, char *pid);

/* read a message on socket */
message* get_msg(int sock_id);

/* send a message on socket */
void send_msg(int sock_id, message_type mtype, char *msg);

#endif // UTILS_H