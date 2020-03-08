/* 
Created by Lukasz Jezapkowicz on 03.04.2020.
*/

#include <my_library.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void create_main_array(int size) {
    if (size <= 0) {
        printf("Can't create array! Size must be positive.\n");
        return;
    }
    main_array = (struct data_block**)calloc(size,sizeof(struct block*));
    blocks_length = (int *)calloc(size,sizeof(int));
    active_blocks_length = (int *)calloc(size,sizeof(int));
    c_index = 0;
}

struct file_pair * make_file_sequence(char *files[], int size) {
    struct file_pair * fsequence = (struct file_pair *)calloc(size,sizeof(struct file_pair));
    char file1[50];
    char file2[50];
    char tmp[50];
    for (int i=0; i<size; i++) {
        strcpy(tmp,files[i]);
        strcpy(file1,strtok(tmp,":"));
        strcpy(file2,strtok(NULL,"\0"));
        struct file_pair tmp_file;
        tmp_file.file1 = (char *)calloc(50,sizeof(char));
        tmp_file.file2 = (char *)calloc(50,sizeof(char));
        strcpy(tmp_file.file1,file1);
        strcpy(tmp_file.file2,file2);
        fsequence[i]=tmp_file;
    }
    return fsequence;
}

void execute_diff(struct file_pair files[],int size) {
    system("touch diff_result");

    for(int i=0; i<size; i++) {
        char command[60];
        snprintf(command,sizeof(command),"diff %s %s > diff_result",files[i].file1,files[i].file2);
        if (system(command)==256)
            printf("Result of \"diff %s %s\" was put to main array at index %d\n",files[i].file1,files[i].file2,put_block(files[i],size));
        else
            printf("One of the following files doesn't exist: %s, %s\n",files[i].file1,files[i].file2);
    }
    system("rm diff_result");
}

int put_block(struct file_pair files, int size) {
    if (c_index==size) {
        printf("Main array is full!.\n");
        return -1;
    }
    FILE *fp = fopen("diff_result","r");
    char line[100];
    int length = 0;
    while(fgets(line,sizeof(line),fp) != NULL) {
        if(line[0]>='0' && line[0]<='9')
            length++;
    }
    blocks_length[c_index]=length;
    active_blocks_length[c_index]=length;
    main_array[c_index] = (struct data_block *)calloc(length,sizeof(struct data_block));
    fp = fopen("diff_result","r");
    char *operation = (char *)calloc(500,sizeof(char));
    int i = 0;
    strcpy(operation,"");
    while(fgets(line,sizeof(line),fp) != NULL) {
        if(line[0]>='0' && line[0]<='9' && strcmp(operation,"") != 0) {
            main_array[c_index][i++].operation = operation;
            operation = (char *)calloc(500,sizeof(char));
            strcpy(operation,line);            
        }
        else {
            strcat(operation,line);
        }
    }
    main_array[c_index][i].operation = operation;
    for (i=0; i<length; i++) main_array[c_index][i].operation = main_array[c_index][i+1].operation;
    return c_index++;
}

int operation_block_size(int index, int size) {
    if (main_array[index]==NULL || index <0 || index >= size) {
        printf("Can't return this block of operations length cause it doesn't exist (is either NULL or index is invalid).\n");
        return -1;
    }
    return active_blocks_length[index];
}

void delete_operation_block(int index, int size) {
    if (main_array[index]==NULL || index < 0 || index >= size) {
        printf("Can't delete this block of operations cause it doesn't exist (is either NULL or index is invalid).\n");
        return;
    }
    free(main_array[index]);
    main_array[index]=NULL;
}

void delete_operation(int block_index, int index, int size) {
    if (main_array[block_index]==NULL || index < 0 || index >= blocks_length[block_index] || block_index < 0 || block_index >= size) {
        printf("Can't delete this operation cause it doesn't exist (is either NULL or indexes are invalid).\n");
        return;
    }
    active_blocks_length[block_index]--;
    free(main_array[block_index][index].operation);
    main_array[block_index][index].operation = NULL;
}
