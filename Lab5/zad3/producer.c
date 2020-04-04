#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
/*
    pipe_path - path to named pipe (FIFO)
    input_path - path to file with goods
    n - number of chars to be read at one time
*/
char *pipe_path;
char *input_path;
int n;

/* program which symbolizes producer */
int main(int argc, char *argv[]) {
    srand(time(NULL));
    /* checking and parsing arguments */
    if (argc != 4) {
        printf("Wrong number of arguments!\n");
        return -1;
    }
    pipe_path = argv[1];
    input_path = argv[2];
    n = atoi(argv[3])+1;

    /* opening named pipe for writing and input file for reading */
    int p = open(pipe_path,O_WRONLY);
    FILE *f = fopen(input_path,"r");
    /* buffer with given size */
    char chunk[n];
    if(f != NULL) {
        /* reading goods and writing it to pipe */
        while (fgets(chunk,n,f) != NULL) {
            char msg[10+n];
            sprintf(msg,"#%d#%s\n",getpid(),chunk);
            write(p,msg,strlen(msg));
            /* wait 1-2 seconds - new product invention */
            sleep(rand()%1+1);
        }
    }
    /* closing fifo and file */
    close(p);
    fclose(f);
    
    return 0;
}