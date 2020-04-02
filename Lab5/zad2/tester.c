#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>

/* program testing the producer and consumer problem */
int main(int argc, char *argv[]) {
    /* name of the named pipe */
    char *pipe_name = "fifo";
    
    /* pid of the processes */
    pid_t pids[6];

    /* creating named pipe in current directory with permission for writing and reading */
    mkfifo(pipe_name,S_IRUSR | S_IWUSR);

    /* creating lists of arguments for 1 consumer and 5 producents processes */
    char *const cons1[] = {"./consumer","fifo","./tests/results","15",NULL};
    char *const prod1[] = {"./producer","fifo","./tests/test1","5",NULL};
    char *const prod2[] = {"./producer","fifo","./tests/test2","5",NULL};
    char *const prod3[] = {"./producer","fifo","./tests/test3","5",NULL};
    char *const prod4[] = {"./producer","fifo","./tests/test4","5",NULL};
    char *const prod5[] = {"./producer","fifo","./tests/test5","5",NULL};

    /* creating processes and testing */

    /* consumer */
    if ((pids[5]=fork()) == 0) 
        execvp(cons1[0],cons1);

    /* producers */
    if ((pids[0]=fork()) == 0)
        execvp(prod1[0],prod1);

    if ((pids[1]=fork()) == 0)
        execvp(prod2[0],prod2);
    
    if ((pids[2]=fork()) == 0)
        execvp(prod3[0],prod3);

    if ((pids[3]=fork()) == 0)
        execvp(prod4[0],prod4);
    
    if ((pids[4]=fork()) == 0)
        execvp(prod5[0],prod5);
    
    /* waiting for processes to end */
    for (int i=0; i<6; i++)
        waitpid(pids[i],NULL,0);

    return 0;
}