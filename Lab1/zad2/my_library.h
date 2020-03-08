/* 
Created by Lukasz Jezapkowicz on 03.04.2020.
*/

#ifndef MY_LIBRARY_H
#define MY_LIBRARY_H

struct data_block{
    char *operation;
};

struct data_block **main_array;
int * blocks_length;
int * active_blocks_length;
int c_index;

struct file_pair{
    char *file1;
    char *file2;
};

void create_main_array(int size);

struct file_pair * make_file_sequence(char *files[], int size);

void execute_diff(struct file_pair files[],int size);

void delete_diff(int size);

int put_block(struct file_pair files, int size, int index);

int operation_block_size(int index, int size);

void delete_operation_block(int index, int size);

void delete_operation(int block_index, int index, int size);

#endif