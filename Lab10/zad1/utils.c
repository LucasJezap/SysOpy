#ifndef UTILS_C
#define UTILS_C

#include "utils.h"

/* handling errors */
void print_error(char *error) {
    DEFAULT();
    printf("Error: %s\n",error);
}

/* start new game */
chess_game* start_new_game(int player1_id, int player2_id) {
    chess_game *game = (chess_game*)calloc(1,sizeof(chess_game));
    game->player1_id = player1_id;
    game->player2_id = player2_id;
    for (int i=0; i<9; i++)
        game->map[i] = NOTHING;
    return game;
}

/* make a move in game */
void chess_move(chess_game *game, square sign, int square_no) {
    game->map[square_no] = sign;
}

/* get a 3x3 board */
char* get_square_map(chess_game *game) {
    char *map = (char *)calloc(300,sizeof(char));
    char tmp1[] = "---------------------\n";
    char tmp2[] = "|     ||     ||     |\n";
    strcpy(map,tmp1);
    for (int i=0; i<3; i++) strcat(map,tmp2);
    strcat(map,tmp1);
    for (int i=0; i<3; i++) strcat(map,tmp2);
    strcat(map,tmp1);
    for (int i=0; i<3; i++) strcat(map,tmp2);
    strcat(map,tmp1);

    int positions[9] = {47,54,61,135,142,149,223,230,237};

    for (int i=0; i<9; i++) {
        if (game->map[i] == O)
            map[positions[i]] = 'O';
        else if (game->map[i] == X)
            map[positions[i]] = 'X';
    }
    return map;
}

/* check if game has already ended */
game_result check_game_result(chess_game *game) {
    int combinations[8][3] = {{0,1,2}, {3,4,5}, {6,7,8}, {0,3,6}, {1,4,7}, {2,5,8}, {0,4,8}, {2,4,6}};
    for (int i=0; i<8; i++) {
        if (game->map[combinations[i][0]] == O && game->map[combinations[i][1]] == O &&
            game->map[combinations[i][2]] == O) return O_W;
        if (game->map[combinations[i][0]] == X && game->map[combinations[i][1]] == X &&
            game->map[combinations[i][2]] == X) return X_W;
    }
    for (int i=0; i<8; i++)
        if (game->map[i] == NOTHING) return UNCOMPLETED;
    return DRAW;
}

/* creating a new client */
client* create_client(int id, char *nickname, char *pid) {
    client *new_client = (client*)calloc(1,sizeof(client));
    new_client->id = id;
    new_client->is_responding = 1;
    new_client->client_pid = (pid_t) atoi(pid);
    strcpy(new_client->nickname,nickname);
    return new_client;
}

/* getting msg type helper */
enum message_type get_mtype(char *msg) {
    enum message_type mtype;
    sscanf(msg, "%d", (int *) &mtype);
    return mtype;
}

char* get_real_msg(char *msg) {
    int tmp;
    char* real_msg = (char*)calloc(MAX_LENGTH_OF_MSG-1,sizeof(char));
    sscanf(msg,"%d:%[^:]",&tmp,real_msg);
    return real_msg;
}

/* read a message on socket */
message* get_msg(int sock_id) {
    message *msg = (message *)calloc(1,sizeof(message));
    char *socket_msg = (char *)calloc(MAX_LENGTH_OF_MSG,sizeof(char));
    if (recv(sock_id,(void *)socket_msg,MAX_LENGTH_OF_MSG,0) == -1) {
        print_error("Error while reading from socket!");
        return NULL;
    }
    msg->mtype = get_mtype(socket_msg);
    strcpy(msg->msg,get_real_msg(socket_msg));
    free(socket_msg);
    return msg;
}

/* send a message on socket */
void send_msg(int sock_id, message_type mtype, char *msg) {
    char *socket_msg = (char *)calloc(MAX_LENGTH_OF_MSG,sizeof(char));
    sprintf(socket_msg,"%d:%s", (int) mtype, msg);
    if (write(sock_id,(void *)socket_msg, MAX_LENGTH_OF_MSG) == -1) {
        print_error("Error while sending message!");
        return;
    }
    free(socket_msg);
}

#endif // UTILS_C