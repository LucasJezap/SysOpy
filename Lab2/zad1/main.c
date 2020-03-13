#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

clock_t start_t, end_t;
struct tms start_cpu, end_cpu;

void timer_start() {
    start_t = times(&start_cpu);
}

void timer_stop() {
    end_t = times(&end_cpu);
}

void print_time(char *op) {
    printf("%-40s",op);
    printf("%-20f",(double) (end_t-start_t)/ sysconf(_SC_CLK_TCK));
    printf("%-20f",(double) (end_cpu.tms_utime-start_cpu.tms_utime) / sysconf(_SC_CLK_TCK));
    printf("%-20f",(double) (end_cpu.tms_stime-start_cpu.tms_stime) / sysconf(_SC_CLK_TCK));
    printf("%-20f",(double) (end_cpu.tms_cutime-start_cpu.tms_cutime) / sysconf(_SC_CLK_TCK));
    printf("%-20f\n",(double) (end_cpu.tms_cstime-start_cpu.tms_cstime) / sysconf(_SC_CLK_TCK));
}

void generate(char * filename, int count, int length) {
    if (count<0 || length<0) {
        printf("size and length must be positive!\n");
        return;
    }
    for (int i=0; i<count; i++) {
        char * command = (char *)calloc(100,sizeof(char));
        sprintf(command,"head /dev/urandom | tr -dc A-Za-z0-9 | head -c %d >> %s",length,filename);
        system(command);
        char * command2 = (char *)calloc(15,sizeof(char));
        sprintf(command2,"echo '' >> %s",filename);
        system(command2);
        free(command);
        free(command2);
    }
}


void libraryCopy(char * source, char * dest, int count, int length) {
    if(count<0 || length<0) {
        printf("count and length must be positive!\n");
        return;
    }
    char buffer[length];
    FILE *s = fopen(source,"r");
    FILE *d = fopen(dest,"w");
    if(!s) {
        printf("Source file doesn't exist!\n");
        return;
    }
    if(!d) return;
    for (int i=0; i<count; i++) {
        fread(buffer,length+1,1,s);
        fwrite(buffer,length+1,1,d);
    }
    fclose(s);
    fclose(d);
}

void systemCopy(char * source, char * dest, int count, int length) {
    if(count<0 || length<0) {
        printf("count and length must be positive!\n");
        return;
    }
    char buffer[length];
    int s = open(source, O_RDONLY);
    if(s == -1) {
        printf("Source file doesn't exist!\n");
        return;
    }
    char create[20];
    sprintf(create,"touch %s",dest);
    system(create);
    int d = open(dest,O_WRONLY);
    for (int i=0; i<count; i++) {
        read(s,buffer,length+1);
        write(d,buffer,length+1);
    }
    close(s);
    close(d);
}

// --------------------------------------------------------------
void librarySwap(FILE * f, int length, int l, int r) {
    char * bufferL = (char *)calloc(length+1,sizeof(char));
    char * bufferR = (char *)calloc(length+1,sizeof(char));
    fseek(f,length*l,SEEK_SET);
    fread(bufferL,length,1,f);
    fseek(f,length*r,SEEK_SET);
    fread(bufferR,length,1,f);

    fseek(f,length*l,SEEK_SET);
    fwrite(bufferR,length,1,f);
    fseek(f,length*r,SEEK_SET);
    fwrite(bufferL,length,1,f);

    free(bufferL);
    free(bufferR);
}

int libraryPartition(FILE * f, int length, int l, int r) {
    fseek(f,length*r, SEEK_SET);
    char * pivot = (char *)calloc(length+1,sizeof(char));
    fread(pivot,length,1,f);
    int i = l - 1;
    int j = l - 1;
    char * buffer = (char *)calloc(length+1,sizeof(char));
    while (++j < r) {
        fseek(f,length*j,SEEK_SET);
        fread(buffer,length,1,f);
        if (strcmp(buffer,pivot) <= 0) {
            librarySwap(f,length,++i,j);
        }
    }
    librarySwap(f,length,i+1,j);
    free(pivot);
    free(buffer);
    return i + 1;
}

void libraryQuickSort(FILE *f, int length, int l , int r) {
    if (l < r) {
        int p = libraryPartition(f,length,l,r);
        libraryQuickSort(f,length,l,p-1);
        libraryQuickSort(f,length,p+1,r);
    }    
}

void librarySort(char * filename, int count, int length) {
    if(count<0 || length<0) {
        printf("count and length must be positive!\n");
        return;
    }
    FILE *f = fopen(filename,"r+");
    if(!f) {
        printf("given file doesn't exist!\n");
        return;
    }
    if(count == 0 || count == 1) return;
    libraryQuickSort(f,length+1,0,count-1);
    fclose(f);
}
// --------------------------------------------------------------

void systemSwap(int f, int length, int l, int r) {
    char * bufferL = (char *)calloc(length+1,sizeof(char));
    char * bufferR = (char *)calloc(length+1,sizeof(char));

    lseek(f,length*l,SEEK_SET);
    read(f,bufferL,length);
    lseek(f,length*r,SEEK_SET);
    read(f,bufferR,length);

    lseek(f,length*l,SEEK_SET);
    write(f,bufferR,length);
    lseek(f,length*r,SEEK_SET);
    write(f,bufferL,length);

    free(bufferL);
    free(bufferR);
}

int systemPartition(int f, int length, int l, int r) {
    lseek(f,length*r, SEEK_SET);
    char * pivot = (char *)calloc(length+1,sizeof(char));
    read(f,pivot,length);
    int i = l - 1;
    int j = l - 1;
    char * buffer = (char *)calloc(length+1,sizeof(char));
    while (++j < r) {
        lseek(f,length*j,SEEK_SET);
        read(f,buffer,length);
        if (strcmp(buffer,pivot) <= 0) {
            systemSwap(f,length,++i,j);
        }
    }
    systemSwap(f,length,i+1,j);
    free(pivot);
    free(buffer);
    return i + 1;
}

void systemQuickSort(int f, int length, int l , int r) {
    if (l < r) {
        int p = systemPartition(f,length,l,r);
        systemQuickSort(f,length,l,p-1);
        systemQuickSort(f,length,p+1,r);
    }    
}

void systemSort(char * filename, int count, int length) {
    if(count<0 || length<0) {
        printf("count and length must be positive!\n");
        return;
    }
    int f = open(filename,O_RDWR);
    if(f == -1) {
        printf("given file doesn't exist!\n");
        return;
    }
    if(count == 0 || count == 1) return;
    systemQuickSort(f,length+1,0,count-1);
    close(f);
}

// --------------------------------------------------------------

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("\nYou need to pass more arguments for program to operate! Possible options:\ngenerate [file] [number] [length]\n"
        "sort [file] [number] [length] [sys | lib]\ncopy [file1] [file2] [number] [length] [sys | lib]\n\n");
        return -1;
    }
    if (strcmp(argv[1],"generate") == 0) {
        if(argc < 5) {
            printf("You need to pass more arguments!\n");
            return -1;
        }
        generate(argv[2],atoi(argv[3]),atoi(argv[4]));
    }
    else if (strcmp(argv[1],"sort") == 0) {
        if(argc < 6) {
            printf("You need to pass more arguments!\n");
            return -1;
        }
        if (strcmp(argv[5],"sys") == 0) {
            timer_start();
            systemSort(argv[2],atoi(argv[3]),atoi(argv[4]));
            timer_stop();
            print_time("sorting system");
        }
        else {
            timer_start();
            systemSort(argv[2],atoi(argv[3]),atoi(argv[4]));
            timer_stop();
            print_time("sorting library");
        }
    }
    else if (strcmp(argv[1],"copy") == 0) {
        if(argc < 7) {
            printf("You need to pass more arguments!\n");
            return -1;
        }
        if (strcmp(argv[6],"sys") == 0) {
            timer_start();
            systemCopy(argv[2],argv[3],atoi(argv[4]),atoi(argv[5]));
            timer_stop();
            print_time("copy system");
        }
        else {
            timer_start();
            libraryCopy(argv[2],argv[3],atoi(argv[4]),atoi(argv[5]));
            timer_stop();
            print_time("copy library");
        }
    }
    else {
        printf("Wrong argument!\n");
        return -1;
    }
    return 0;
}