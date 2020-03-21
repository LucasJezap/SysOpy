#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char *file;
int number_of_pairs;
int mode;


void get_matrixes(int n, int **A, int **B, int **C) {
    FILE *l = fopen(file,"r");
    int i = 0;
    char buffer[500];
    while(i++<n && (fgets(buffer,sizeof(buffer),l) != NULL));
    FILE *a = fopen(strtok(buffer," "),"r");
    FILE *b = fopen(strtok(NULL," "),"r");
    FILE *c = fopen(strtok(NULL,"\n"),"r");
    int rows1 = 0;
    int rows2 = 0;
    int columns2 = 1;
    while (fgets(buffer,sizeof(buffer),a) != NULL) rows1++;
    while (fgets(buffer,sizeof(buffer),b) != NULL) rows2++;
    fseek(a,0,SEEK_SET);
    fseek(b,0,SEEK_SET);
    fgets(buffer,sizeof(buffer),b);
    for(int j=0; j<strlen(buffer); j++)
        if(buffer[j] == ' ') columns2++;
    fseek(b,0,SEEK_SET);
    int check1 = 0;
    int check2 = 0;
    while (fgets(buffer,sizeof(buffer),c) != NULL) check1++;
    for(int j=0; j<strlen(buffer); j++)
        if(buffer[j] == ' ') check2++;
    if(check1 != rows1) {
        printf("The multiplication didn't manage to end in given time! Give it more time please!\n");
        return;
    }
    fseek(c,0,SEEK_SET);
    A = (int **)calloc(rows1,sizeof(int *));
    B = (int **)calloc(rows2,sizeof(int *));
    C = (int **)calloc(rows1,sizeof(int *));
    for(int j=0; j<rows1; j++) {
        A[j] = (int *)calloc(rows2,sizeof(int));
        C[j] = (int *)calloc(columns2,sizeof(int));
    }
    for(int j=0; j<rows2; j++) B[j] = (int *)calloc(columns2,sizeof(int));
    int j = 0;
    while(fgets(buffer,sizeof(buffer),a) != NULL) {
        int c;
        for(int k=0; k<rows2; k++) {
            if (k == 0) c = atoi(strtok(buffer," "));
            else if(k == rows2-1) c = atoi(strtok(NULL,"\n"));
            else c = atoi(strtok(NULL," "));
            A[j][k] = c;
        }
        j++;
    }
    j=0;
    while(fgets(buffer,sizeof(buffer),b) != NULL) {
        int c;
        for(int k=0; k<columns2; k++) {
            if (k == 0) c = atoi(strtok(buffer," "));
            else if(k == columns2-1) c = atoi(strtok(NULL,"\n"));
            else c = atoi(strtok(NULL," "));
            B[j][k] = c;
        }
        j++;
    }
    j=0;
    while(fgets(buffer,sizeof(buffer),c) != NULL) {
        char *buffer2 = buffer;
        while(isspace(*buffer2)) buffer2++;
        int c;
        for(int k=0; k<columns2; k++) {
            if(k == 0) c = atoi(strtok(buffer2," "));
            else if (k == columns2-1) c = atoi(strtok(NULL,"\n"));
            else c = atoi(strtok(NULL," "));
            C[j][k] = c;
        }
        j++;
    }
    fclose(a);
    fclose(b);
    fclose(c);

    /* checking */
    int error = 0;
    for (int j=0; j<rows1; j++) {
        for(int k=0; k<columns2; k++) {
            int res = 0;
            for (int l=0; l<rows2; l++) 
                res += A[j][l]*B[l][k];
            if (res != C[j][k]) error = 1;
            if(error == 1) break;
        }
        if(error == 1) break;
    }
    if (error == 1) printf("The multiplication of matrixes no %d isn't correct!\n",n);
    else printf("The multiplication of matrixes no %d on the list went correctly!\n",n);
}

int main(int argc, char *argv[]) {
    if(argc != 4) {
        printf("WRONG NUMBER OF PARAMETERS!\n");
        return -1;
    }
    
    file = argv[1];
    number_of_pairs=atoi(argv[2]);
    mode=atoi(argv[3]);

    for(int i=0; i<number_of_pairs; i++) {
        int **A;
        int **B;
        int **C;
        get_matrixes(i+1,A,B,C);
    }
    printf("\n\n\n");
    return 0;
}