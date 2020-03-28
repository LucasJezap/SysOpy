#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int catcher_pid;
int catching_in_progress = 1;
int number_of_signals;
int catched_signals = 0;
int last_received = -1;

void handler_1(int signum,siginfo_t *info, void *secret) {
    catched_signals++;
    return;
}

void handler_2(int signum,siginfo_t *info, void *secret) {
    catching_in_progress = 0;
    return;
}

void handler_2_bonus(int signum,siginfo_t *info, void *secret) {
    catching_in_progress = 0;
    last_received = info->si_int;   
    return;
}


/* sender - program sending SIGUSR1 and SIGUSR2 signals to catcher */
int main(int argc, char *argv[]) {
    /* checking and parsing arguments */
    if (argc != 4) {
        printf("Wrong number of arguments!\n");
        return -1;
    }
    catcher_pid = atoi(argv[1]);
    number_of_signals = atoi(argv[2]);

    struct sigaction act1,act2;
    /* sending signals and setting appropriate handlers */
    if (strcmp(argv[3],"kill") == 0) {
        for(int i=0; i<number_of_signals; i++)
            kill(catcher_pid,SIGUSR1);
        kill(catcher_pid,SIGUSR2);
        /* handlers */
        sigemptyset(&act1.sa_mask);
        sigemptyset(&act2.sa_mask);
        act1.sa_flags = 0;
        act2.sa_flags = SA_SIGINFO;
        act1.sa_sigaction = handler_1;
        act2.sa_sigaction = handler_2;        
        sigaction(SIGUSR1,&act1,NULL);
        sigaction(SIGUSR2,&act2,NULL);
    }
    else if (strcmp(argv[3],"sigqueue") == 0) {
        union sigval s;
        for(int i=0; i<number_of_signals; i++) {
            s.sival_int = i;
            sigqueue(catcher_pid,SIGUSR1,s);
        }
        s.sival_int = number_of_signals;
        sigqueue(catcher_pid,SIGUSR2,s);
        /* handlers */
        sigemptyset(&act1.sa_mask);
        sigemptyset(&act2.sa_mask);
        act1.sa_flags = 0;
        act2.sa_flags = SA_SIGINFO;
        act1.sa_sigaction = handler_1;
        act2.sa_sigaction = handler_2_bonus;        
        sigaction(SIGUSR1,&act1,NULL);
        sigaction(SIGUSR2,&act2,NULL);
    }
    else if (strcmp(argv[3],"sigrt") == 0) {
        for(int i=0; i<number_of_signals; i++) 
            kill(catcher_pid,SIGRTMIN+2);
        kill(catcher_pid,SIGRTMIN+3);
        /* handlers */
        sigemptyset(&act1.sa_mask);
        sigemptyset(&act2.sa_mask);
        act1.sa_flags = 0;
        act2.sa_flags = SA_SIGINFO;
        act1.sa_sigaction = handler_1;
        act2.sa_sigaction = handler_2;        
        sigaction(SIGRTMIN+2,&act1,NULL);
        sigaction(SIGRTMIN+3,&act2,NULL);
    }
    /* catching return signals */
    while(catching_in_progress) {
        pause();
    }

    /* closing message */
    printf("Sender should receive %d signals\n",number_of_signals);
    printf("Sender received %d signals\n",catched_signals);
    if (last_received != -1)
        printf("Catcher send %d signals\n",last_received);

    return 0;
}