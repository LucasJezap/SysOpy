#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

/* flag */

int stop = 0;

/* function for dealing with SIGTSTP */
void sig_maintenance1 (int signum) {
    if(stop == 0)
        printf("\n\nWaiting for CTRL+Z - process continuation or CTRL+C - end of program\n\n");
    stop = (stop + 1) % 2;
    return;
}

/* function for dealing with SIGINT */
void sig_maintenance2 (int signum) {
    printf("\nSIGINT signal was delivered\n");
    exit(EXIT_SUCCESS);
}

int main() {
    struct sigaction action;
    action.sa_handler = sig_maintenance1;
    action.sa_flags = 0;

    /* infinite loop which prints current directory */
    while(1) {
        sigaction(SIGTSTP,&action,NULL);
        signal(SIGINT,sig_maintenance2);
        if (!stop) {
            system("ls -l");
            sleep(1);
        }
    }

    return 0;
}