#include <stdio.h>

/* sort a file using popen pipe stream */
int main(int argc, char *argv[]) {
    /* checking and parsing arguments */
    if (argc != 2) {
        printf("Wrong number of arguments!\n");
        return -1;
    }
    
    /* preparing command to execute */
    char command[25];
    sprintf(command,"sort %s",argv[1]);

    /* executing command via another process using popen function */
    FILE *x = popen(command,"w");
    /* closing pipe stream */
    pclose(x);
    
    return 0;
}