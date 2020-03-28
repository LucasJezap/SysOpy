#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* 
    i - ignore
    m - mask
    p - pending */
char mode;

/* Testing signal inheritance of exec on SIGUSR1 */
int main(int argc, char * argv[]) {
    if(strcmp(argv[1],"ignore") == 0) mode = 'i';
    else if (strcmp(argv[1],"mask") == 0) mode = 'm';
    else if (strcmp(argv[1],"pending") == 0) mode = 'p';
    else { printf("Wrong argument!\n"); return -1; }


    /* looking if process after exec sees the raised signal */
    if(mode == 'p') {
    sigset_t pending;
    sigpending(&pending);
    if (sigismember(&pending,SIGUSR1)) 
        printf("Signal SIGUSR1 is in pending state in the process after being raised before exec command\n");
    else
        printf("Signal SIGUSR1 is not in pending state in the process after being raised before exec command\n");
    }

    /* process raises signal to itself to test if worked correctly */
    raise(SIGUSR1);
    
    /* checking what happened after raise */
    if (mode == 'i') {
        printf("The process is still running so ignoring the signal SIGUSR1 worked!\n");
    }
    else if (mode == 'm') {
        printf("The process is still running so blocking the signal SIGUSR1 worked!\n");
    }
    else if(mode == 'p') {
        sigset_t pending;
        sigpending(&pending);
        if (sigismember(&pending,SIGUSR1)) 
            printf("After raising in process after exec, signal SIGUSR1 is in pending state in the process\n");
        else
            printf("After raising in process after exec, signal SIGUSR1 is not in pending state in the process\n");
    } 

    return 0;
}