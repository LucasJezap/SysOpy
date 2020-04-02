#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE_SIZE 1024

/* pipe interpreting programme - popen version */
int main(int argc, char *argv[]) {
    /* checking and parsing arguments */
    if (argc != 2) {
        printf("Wrong number of arguments!\n");
        return -1;
    }
    FILE *f;
    /* opening file given by argument */
    if ((f = fopen(argv[1],"r")) == NULL) {
        printf("Given file path is wrong!\n");
        return -1;
    }
    /* buffer for line */
    char line[MAX_LINE_SIZE];

    while(fgets(line,MAX_LINE_SIZE,f) != NULL) {
        /* right part of the interpreter */
        printf("Now interpreting line:\n%s\nThe result of the command:\n",line);
        FILE *x = popen(line,"w");
        pclose(x);
    }
    /* closing the file */
    fclose(f);
    
    return 0;
}