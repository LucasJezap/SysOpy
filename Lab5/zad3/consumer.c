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
    result_path - path to file to save read products
    n - number of chars to be read at one time
*/
char *pipe_path;
char *result_path;
int n;

/* program which symbolizes consumer */
int main(int argc, char *argv[]) {
    /* checking and parsing arguments */
    if (argc != 4) {
        printf("Wrong number of arguments!\n");
        return -1;
    }
    pipe_path = argv[1];
    result_path = argv[2];
    n = atoi(argv[3])+1;

    /* opening named pipe for reading and input file for writing */
    FILE *p = fopen(pipe_path,"r");
    FILE *f = fopen(result_path,"w");
    /* buffer with given size */
    char chunk[n];
    if(f != NULL) {
        /* reading goods from pipe and saving it to result */
        while (fgets(chunk,n,p) != NULL) {
            fprintf(f,"%s",chunk);
        }
    }

    /* closing fifo and file */
    fclose(p);
    fclose(f);

    return 0;
}