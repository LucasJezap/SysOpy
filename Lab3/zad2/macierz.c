#define _POSIX_C_SOURCE 1
#include <stdlib.h>
#include <stdio.h>
#include <sys/file.h>
#include <sys/times.h>
#include <errno.h>
#include <libgen.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

FILE *l;
int max_seconds;
int number_of_processes;
int number_of_pairs;
int *result_rows;
int *matrix_sizes;
int *matrix_jumps;

clock_t start_t, end_t;
struct tms start_cpu, end_cpu;

void timer_start() {
    start_t = times(&start_cpu);
}

void timer_stop() {
    end_t = times(&end_cpu);
}

int process_child(int n, int mode) {
    timer_start();
    int multiplications = 0;
    fseek(l,0,SEEK_SET);
    char buffer[500];
    int i = 0;
    int res;
    int line;
    if (l != NULL) {
        while(fgets(buffer,sizeof(buffer),l) != NULL && i<number_of_pairs) {
            FILE *x = fopen(strtok(buffer," "),"r");
            FILE *y = fopen(strtok(NULL," "),"r");
            char *z = strtok(NULL,"\n");
            char buffer2[500];
            fseek(x,0,SEEK_SET);
            timer_stop();
            if((double)(end_t-start_t)/sysconf(_SC_CLK_TCK) > max_seconds)
                exit(multiplications);
            if (x != NULL) {
                line = 0;
                while(fgets(buffer2,sizeof(buffer2),x) != NULL) {
                    int k=0;
                    int j = n*matrix_jumps[i];
                    while (k++ < matrix_jumps[i]) {
                        res = 0;
                        fseek(y,0,SEEK_SET);
                        int count = 0;
                        if (y != NULL) {
                            char buffer4[500];
                            while(fgets(buffer4,sizeof(buffer4),y) != NULL) {
                                char buffer3[500];
                                strcpy(buffer3,buffer2);
                                int m;
                                char *column_number;
                                for(m = 0; m<=j; m++) {
                                    if(m==0) column_number = strtok(buffer4," ");
                                    else column_number = strtok(NULL," ");
                                }
                                char *row_number;
                                int n;
                                for(n = 0; n<=count; n++) {
                                    if(n==0) row_number = strtok(buffer3," ");
                                    else row_number = strtok(NULL," ");
                                }
                           
                                res += atoi(column_number)*atoi(row_number);
                                count++;
                            }
                        }
                        if(mode == 1) {
                            char command[50];
                            sprintf(command,"sed -i '%ds/$/ %d/' %s",line+1,res,z);
                            system(command);
                        }
                        else {
                            char command[50];
                            sprintf(command,"echo -n '%d ' >> result_%d_%d",res,i,line);
                            system(command);
                        }
                        j++;
                        if(j == matrix_sizes[i]) break;
                    }
                    line++;    
                }
            }

            i++;
            multiplications++;
            fclose(x);
            fclose(y);
        }
    }
    return multiplications;
}


void calculate_number() {
    fseek(l,0,SEEK_SET);
    char buffer[500];
    number_of_pairs = 0;
    if(l != NULL) {
        while(fgets(buffer,sizeof(buffer),l) != NULL)
            number_of_pairs++;
    }
    result_rows = (int *)calloc(number_of_pairs,sizeof(int));
    matrix_jumps = (int *)calloc(number_of_pairs,sizeof(int));
    matrix_sizes = (int *)calloc(number_of_pairs,sizeof(int));
    for(int i=0; i<number_of_pairs; i++) {
        matrix_sizes[i]=1;
        result_rows[i]=0;
    }
}

void calculate_sizes() {
    fseek(l,0,SEEK_SET);
    char buffer[500];
    int i=0;
    if(l != NULL) {
        while(fgets(buffer,sizeof(buffer),l) != NULL) {
            FILE *a = fopen(strtok(buffer," "),"r");
            FILE *b = fopen(strtok(NULL," "),"r");
            if(a != NULL) {
                while(fgets(buffer,sizeof(buffer),a) != NULL) result_rows[i]++;
                fclose(a);
            }
            if(b != NULL) {
                fgets(buffer,sizeof(buffer),b);
                for(int j=0; j<strlen(buffer); j++)
                    if(buffer[j] == ' ') matrix_sizes[i]++;
                fclose(b);
            }
            matrix_jumps[i] = matrix_sizes[i]/number_of_processes;
            if(matrix_sizes[i]%number_of_processes !=0) matrix_jumps[i]++;
            i++;
        }
    }
}

void write_to_file() {
    for (int i=0; i<number_of_pairs; i++) {
            char command[500];
            strcpy(command,"paste -s ");
            char tmp[50];
            for(int j=0; j<result_rows[i]; j++) {
                sprintf(tmp,"result_%d_%d ",i,j);
                strcat(command,tmp);
            }
            sprintf(tmp," >> m%d_c",i+1);
            strcat(command,tmp);
            
            system(command);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Wrong number of arguments! Provide it like this\n./macierz [list] [number_of_processes] [max_time] [mode]\n");
        return -1;
    }

    l = fopen(argv[1],"r");
    number_of_processes = atoi(argv[2]);
    max_seconds = atoi(argv[3]);

    pid_t pids[number_of_processes];
    pid_t statuses[number_of_processes];

    calculate_number();
    calculate_sizes();

    if(strcmp(argv[4],"1") == 0) {
        fseek(l,0,SEEK_SET);
        char buffer[500];
        int j = 0;
        if (l != NULL)
            while(fgets(buffer,sizeof(buffer),l) != NULL) {
                strtok(buffer," ");
                strtok(NULL," ");
                char *file = strtok(NULL,"\n");
                char command[50];
                sprintf(command,"echo '' >> %s",file);
                for(int i=0; i<result_rows[j]; i++)
                    system(command);
                j++;
            }
    }

    for(int i=0; i<number_of_processes; i++) {
        pid_t child = fork();
        if (child == -1) {
           printf("Fork error!\n");
           return -1;
        }
        else if(child > 0) pids[i]=child;
        else {
            exit(process_child(i,atoi(argv[4])));
        }
        pids[i]=waitpid(pids[i],&statuses[i],0);
    }

    for(int i=0; i<number_of_processes; i++) {
   
        printf("The process with PID = %d have made %d multiplications\n",(int)pids[i],WEXITSTATUS(statuses[i]));
    }

    if(strcmp(argv[4],"0") == 0)
        write_to_file();

    fclose(l);
    printf("\n");
    return 0;
}