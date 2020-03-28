#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* 
    i - ignore
    h - handler
    m - mask
    p - pending */
char mode;

void handler(int signum) {
    printf("Signal SIGUSR1 was catched\n");
    return;
}

/* Testing signal inheritance of fork on SIGUSR1 */
int main(int argc, char * argv[]) {
    /* parsing arguments */
    if (argc != 2) {
        printf("Wrong number of arguments!\n");
        return -1;
    }
    if(strcmp(argv[1],"ignore") == 0) { mode = 'i'; printf("\nIGNORE\n"); }
    else if (strcmp(argv[1],"handler") == 0) { mode = 'h'; printf("\nHANDLER\n"); }
    else if (strcmp(argv[1],"mask") == 0) { mode = 'm'; printf("\nMASK\n"); }
    else if (strcmp(argv[1],"pending") == 0) { mode = 'p'; printf("\nPENDING\n"); }
    else { printf("Wrong argument!\n"); return -1; }
    
    /* doing appropriate work to given argument */
    printf("Now testing for parent process:\n");
    if (mode == 'i') {
        signal(SIGUSR1,SIG_IGN);
    }
    else if (mode == 'h') {
        struct sigaction act;
        act.sa_handler = handler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaction(SIGUSR1,&act,NULL);
    }
    else if (mode == 'm' || mode == 'p') {
        /* making set of signals containing only SIGUSR1 */
        sigset_t new_mask;
        sigset_t old_mask;
        sigemptyset(&new_mask);
        sigaddset(&new_mask,SIGUSR1);
        if (sigprocmask(SIG_BLOCK,&new_mask,&old_mask) < 0)
            perror("Couldn't block signal SIGUSR1\n");
        
        /* checking if signal is in pending state in process */
        if (mode == 'p') {
            sigset_t pending;
            sigpending(&pending);
            if (sigismember(&pending,SIGUSR1)) 
                printf("Signal SIGUSR1 is in pending state in the parent process after adding him to mask\n");
            else
                printf("Signal SIGUSR1 is not in pending state in the parent process after adding him to mask\n");
        }
    }


    /* process raises signal to itself to test if worked correctly */
    raise(SIGUSR1);
    
    /* checking what happened after raise (without handler, it will print something itself) */
    if (mode == 'i') {
        printf("The parent process is still running so ignoring the signal SIGUSR1 worked!\n");
    }
    else if (mode == 'm') {
        printf("The parent process is still running so blocking the signal SIGUSR1 worked!\n");
    }
    else if(mode == 'p') {
        sigset_t pending;
        sigpending(&pending);
        if (sigismember(&pending,SIGUSR1)) 
            printf("After raising, signal SIGUSR1 is in pending state in the parent process\n");
        else
            printf("After raising, signal SIGUSR1 is not in pending state in the parent process\n");
    }

    /* now checking what will happen with all those settings in child process */
    printf("Now testing for child process:\n");
    pid_t child = fork();

    if(child == 0) {
        /* looking if child process sees the raised signal in parent process after raising */
        if(mode == 'p') {
        sigset_t pending;
        sigpending(&pending);
        if (sigismember(&pending,SIGUSR1)) 
            printf("Signal SIGUSR1 is in pending state in the child process after being raised in parent process\n");
        else
            printf("Signal SIGUSR1 is not in pending state in the child process after being raised in parent process\n");
        }

        /* child process raises signal to itself to test if worked correctly */
        raise(SIGUSR1);
        
        /* checking what happened after raise (without handler, it will print something itself) */
        if (mode == 'i') {
            printf("The child process is still running so ignoring the signal SIGUSR1 worked!\n");
        }
        else if (mode == 'm') {
            printf("The child process is still running so blocking the signal SIGUSR1 worked!\n");
        }
        else if(mode == 'p') {
            sigset_t pending;
            sigpending(&pending);
            if (sigismember(&pending,SIGUSR1)) 
                printf("After raising in child process, signal SIGUSR1 is in pending state in the child process\n");
            else
                printf("After raising in child process, signal SIGUSR1 is not in pending state in the child process\n");
        } 
        exit(0);
    }
    wait(NULL);

    return 0;
}