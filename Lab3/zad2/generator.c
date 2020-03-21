#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

int min,max,number = -1;

void generate() {
    system("touch list");
    FILE *l = fopen("list","w");
    char f1[15],f2[15],f3[15],full[45];
    char c1[30],c2[30],c3[30];
    time_t t;
    srand((unsigned) time(&t));

    for (int i=0; i<number; i++) {
        sprintf(f1,"m%d_a",i+1);
        sprintf(f2,"m%d_b",i+1);
        sprintf(f3,"m%d_c",i+1);
        sprintf(full,"%s %s %s\n",f1,f2,f3);
        fwrite(full,1,strlen(full),l);

        sprintf(c1,"touch %s",f1);
        sprintf(c2,"touch %s",f2);
        sprintf(c3,"touch %s",f3);
        system(c1),system(c2),system(c3);
        int a,b,c;
        a = rand() % (max-min+1) + min;
        b = rand() % (max-min+1) + min;
        c = rand() % (max-min+1) + min;
        char row[(b>c)? b*10: c*10],num[9];

        for (int i=0; i<a; i++) {
            strcpy(row,"");
            for(int j=0; j<b; j++) {
                sprintf(num,"%d",-100+rand()%201);
                strcat(row,num);
                if(j != b-1) strcat(row," ");
            }
            sprintf(c1,"echo %s >> %s",row,f1);
            system(c1);
        }
        for (int i=0; i<b; i++) {
            strcpy(row,"");
            for(int j=0; j<c; j++) {
                sprintf(num,"%d",-100+rand()%201);
                strcat(row,num);
                if(j != c-1) strcat(row," ");
            }
            sprintf(c1,"echo %s >> %s",row,f2);
            system(c1);
        }
    }

    fclose(l);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Too few parameters provided!\nPlease provide parameters like that:\n./generator [number] [min] [max]\n");
        return -1;
    }
    number = atoi(argv[1]);
    min = atoi(argv[2]);
    max = atoi(argv[3]);
    
    if (min > max) {
        int tmp = min;
        min = max;
        max = tmp;
    }

    generate();
    return 0;
}