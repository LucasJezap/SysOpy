#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE_SIZE 1024
#define MAX_COMMAND_NUMBER 16
#define MAX_ARGUMENT_NUMBER 8
#define MAX_ARGUMENT_SIZE 16

typedef struct line_command {
    char *command;
    int argc;
    char **argv;
    pid_t pid;
} line_command;


/* pipe interpreting programme - exec/fork/pipe version */
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
        printf("Now interpreting line:\n%s\nThe result of the command:\n",line);
        /* creating and allocating array of pointers to line_command structures */
        line_command **commands = (line_command **)calloc(MAX_COMMAND_NUMBER,sizeof(line_command *));
        for (int i=0; i<MAX_COMMAND_NUMBER; i++) {
            commands[i] = (line_command *)calloc(1,sizeof(line_command));
            commands[i]->command = (char *)calloc(MAX_ARGUMENT_NUMBER*MAX_ARGUMENT_SIZE,sizeof(char));
            commands[i]->argc = 1;
            commands[i]->argv = (char **)calloc(MAX_ARGUMENT_NUMBER,sizeof(char *));
            for (int j=0; j<MAX_ARGUMENT_NUMBER; j++)
                commands[i]->argv[j] = (char *)calloc(MAX_ARGUMENT_SIZE,sizeof(char));
        }

        /* parsing line to our commands array */
        char * cmd = strtok(line,"|");
        int cnt = 0;
        while(cmd != NULL) {
            /* trim */
            while(isspace(*cmd)) cmd++;
            if(cmd[strlen(cmd)-1] == '\n') cmd[strlen(cmd)-1] = '\0';
            /* parsing command to line_command struct */
            strcpy(commands[cnt++]->command,cmd);
            cmd = strtok(NULL,"|");
        }
        
        /* parsing commands to arguments */
        for (int i=0; i<cnt; i++) {
            int j=0;
            char *arg = strtok(commands[i]->command," ");
            while (arg != NULL) {
                strcpy(commands[i]->argv[j++],arg);
                arg = strtok(NULL," ");
            }
            commands[i]->argc = j;
        }

        /* right part of the interpreter */
        int old_pipe[2];
        int new_pipe[2];
        
        for(int i=0; i<cnt; i++) {
            /* creating new pipe */
            if (i<cnt-1 && pipe(new_pipe) < 0) {
                printf("Error making new pipe\n");
                return -1;
            }
            /* forking new process */
            pid_t pid = fork();
            if (pid < 0) {
                printf("Error while forking new process!\n");
                return -1;
            }
            /* child */
            else if (pid == 0) {
                /* redirecting output if it's not the last command */
                if (i < cnt-1) {
                    dup2(new_pipe[1],STDOUT_FILENO);
                    close(new_pipe[0]);
                    close(new_pipe[1]);
                }
                /* redirecting input if it's not the first command */
                if (i > 0) {
                    dup2(old_pipe[0],STDIN_FILENO);
                    close(old_pipe[0]);
                    close(old_pipe[1]);
                }
                /* NULLING last argument and executing the command */
                commands[i]->argv[commands[i]->argc] = NULL;
                execvp(commands[i]->argv[0],commands[i]->argv);
                exit(0);
            }
            /* parent */
            else {
                commands[i]->pid = pid;
                /* closing old pipe if it's not the first command */
                if (i > 0) {
                    close(old_pipe[0]);
                    close(old_pipe[1]);
                }
                /* changing old pipe to new pipe it it's not the last command */
                if (i < cnt-1) {
                    old_pipe[0] = new_pipe[0];
                    old_pipe[1] = new_pipe[1];
                }
                
            }
        }

        /* waiting for processes to end */
        for (int i=0; i<cnt; i++)
            waitpid(commands[i]->pid,NULL,0);

        /* freeing memory */
        for (int i=0; i<MAX_COMMAND_NUMBER; i++) {
            for(int j=0; j<MAX_ARGUMENT_NUMBER; j++) 
                free(commands[i]->argv[j]);
            free(commands[i]->command);
            free(commands[i]->argv);
            free(commands[i]);
        }
        free(commands);
    }
    /* closing the file */
    fclose(f);
    
    return 0;
}