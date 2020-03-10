#include <stdio.h>
#include <stdlib.h>
#include "lib.h"
#include <sys/times.h>
#include <time.h>
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

void generate_text(char *similarity, char *length, int change) {
    char * text;
    int size;
    char *chunks[4];
    chunks[0]="aaa";
    chunks[1]="bbb";
    chunks[2]="ccc";
    chunks[3]="ddd";
    if (strcmp(length,"small")==0) size = 100;
    else if (strcmp(length,"medium")==0) size = 500;
    else size = 2000;
    text = (char *)calloc(2000,sizeof(char));
    strcpy(text,"");
    if (strcmp(similarity,"similar") == 0) {
        for (int i=0; i<size; i++)  {
            strcat(text,chunks[rand()%4]);
            strcat(text,"\n");
        }
    }
    else if (strcmp(similarity,"different") == 0) {
        for (int i=0; i<size; i++) {
            if(rand()%2==0)
                strcat(text,chunks[rand()%4]);
            else
                strcat(text,chunks[change%4]);
            strcat(text,"\n");
        }
    }
    else {
        for (int i=0; i<size; i++) {
            strcat(text,chunks[change%4]);
            strcat(text,"\n");
        }
    }
    char echo[10000];
    char number[10];
    sprintf(number,"%d",change);
    strcpy(echo,"echo '");
    strcat(echo,text);
    strcat(echo,"' > tmp");
    strcat(echo,number);
    system(echo);
    free(text);
}

void generate_files(struct file_pair files[], int number_of_pairs, char * similarity, char * length) {
    timer_start();
    for (int i=0; i<2*number_of_pairs; i++) {
        char command[20];
        char number[10];
        sprintf(number,"%d",i);
        strcpy(command,"touch tmp");
        strcat(command,number);
        system(command);

        generate_text(similarity,length,i);

        char *file = (char *)calloc(15,sizeof(char));
        strcpy(file,"tmp");
        strcat(file,number);
        if (i%2 == 0)
            files[i/2].file1 = file;
        else
            files[i/2].file2 = file;
    }
    timer_stop();
    print_time("generating files");
    timer_start();
    execute_diff(files,number_of_pairs);
    timer_stop();
    print_time("executing diff");
    timer_start();
    for (int i=0;i<number_of_pairs; i++)
        put_block(files[i],number_of_pairs,i);
    timer_stop();
    print_time("putting results into main array");
    timer_start();
    for (int i=0; i<number_of_pairs; i++) 
        delete_operation_block(i,number_of_pairs);
    timer_stop();
    print_time("deleting allocated blocks");
    timer_start();
    for (int i=0; i<number_of_pairs; i++) {
        put_block(files[i],number_of_pairs,i);
        delete_operation_block(number_of_pairs+i,number_of_pairs);
    }
    timer_stop();
    print_time("adding and deleting blocks");
    timer_start();
     for (int i=0; i<2*number_of_pairs; i++) {
        char command[20];
        char number[10];
        sprintf(number,"%d",i);
        strcpy(command,"rm tmp");
        strcat(command,number);
        system(command);
    }
    timer_stop();
    print_time("deleting generated files");
    delete_diff(number_of_pairs);
    free_memory();
}

void make_operation(char *op, char *arg, int size) {
    if (strcmp(op,"compare_pairs") == 0) {
        char *files[100];
        int i = 0;
        char *token = strtok(arg," ");
        while (token != NULL) {
            files[i] = (char *)calloc(50,sizeof(char));
            strcpy(files[i++],token);
            token = strtok(NULL, " ");
        }
        struct file_pair * file_pairs = make_file_sequence(files,i);
        execute_diff(file_pairs,i);
    }
    else if (strcmp(op,"remove_block") == 0) {
        delete_operation_block(atoi(arg),size);
    }
    else if (strcmp(op,"remove_operation") == 0) {
        char block_index[20];
        char index[20];
        char tmp[50];
        strcpy(tmp,arg);
        strcpy(block_index,strtok(tmp," "));
        strcpy(index,strtok(NULL,"\0"));
        delete_operation(atoi(block_index),atoi(index),size);
    }
    else if (strcmp(op,"generate_and_test")==0) {
        char number_of_pairs[10];
        char length[10];
        char similarity[20];
        char tmp[20];
        strcpy(tmp,arg);
        strcpy(number_of_pairs,strtok(tmp," "));
        strcpy(similarity,strtok(NULL," "));
        strcpy(length,strtok(NULL,"\0"));
        struct file_pair files[atoi(number_of_pairs)];
        generate_files(files,atoi(number_of_pairs),similarity,length);
    }
     else 
        printf("WRONG OPERATION NAME!\n");
}

int main(int argc, char *argv[]) {
    printf("------------------------------------------------------------------\n");
    if (argc<3 || strcmp(argv[1],"create_table") != 0 || argv[2]<=0 || argc>7 || argc%2==0) {
        printf("You didn't provide valid arguments to work with!\nYou should pass arguments i.e.:\n"
                 "./zad2 create_table [size] compare_pairs \"file1A.txt:file1B.txt file2A.txt:file2B.txt\"\n"
                 "Right now the program allows only four operations which are:\ncreate_table [size] (obligatory -> must be provided first)\n"
                 "compare_pairs \"file1A.txt:file1B.txt file2A.txt:file2B.txt\"\nremove_block [index]\nremove_operation "
                 "\"[block_index] [operation_index]\"\ngenerate_and_test \"[number of pairs] [similar\\different\\totally_different] "
                 "[small\\medium\\large]\""
                 "\nPlease choose only  of them\n");
    }
    else {
        printf("%-40s%-20s%-20s%-20s%-20s%-20s\n","Action","real time","utime","stime","cutime","cstime");
        timer_start();
        create_main_array(atoi(argv[2]));
        timer_stop();
        print_time("main array initialization");
        char *first_o,*second_o;

        if (argc > 4) {
            first_o = argv[3];
            timer_start();
            make_operation(first_o,argv[4],atoi(argv[2]));
            timer_stop();
            if (strcmp(first_o,"generate_and_test") != 0)
                print_time(first_o);
        }

        if (argc > 6) {
            second_o = argv[5];
            timer_start();
            make_operation(second_o,argv[6],atoi(argv[2]));
            timer_stop();
            print_time(second_o);
        }
    }
    printf("------------------------------------------------------------------\n");
    return 0;
}