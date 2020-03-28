#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int catching_in_progress = 1;
int catched_signals = 0;
int sender_pid;

void handler_1(int signum,siginfo_t *info, void *secret) {
    catched_signals++;
    return;
}

void handler_2(int signum, siginfo_t *info, void *secret) {
    catching_in_progress = 0;
    sender_pid = info->si_pid;
    return;
}


/* catcher - program catching SIGUSR1 and SIGUSR2 signals from sender */
int main(int argc, char *argv[]) {
    /* checking and parsing arguments */
    if (argc != 2) {
        printf("Wrong number of arguments!\n");
        return -1;
    }
    /* opening message */
    printf("\nCatcher program is ready to work!\nMy PID = %d\n",getpid());

    /* blocking all signals */
    sigset_t new_mask;
    sigset_t old_mask;
    sigfillset(&new_mask);
    if (sigprocmask(SIG_BLOCK,&new_mask,&old_mask) < 0)
        perror("Couldn't block signals\n");
        
    struct sigaction act1,act2;
    /* choosing the mode */
    if (strcmp(argv[1],"sigrt") == 0) {
         /* unblocking appropriate signals */
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask,SIGRTMIN+2);
        sigaddset(&mask,SIGRTMIN+3);
        if (sigprocmask(SIG_UNBLOCK,&mask,NULL) < 0)
            perror("Couldn't unblock signals\n");
        /* setting handlers */
        sigemptyset(&act1.sa_mask);
        sigemptyset(&act2.sa_mask);
        act1.sa_flags = 0;
        act2.sa_flags = SA_SIGINFO;
        act1.sa_sigaction = (void *)handler_1;
        act2.sa_sigaction = (void *)handler_2;        
        sigaction(SIGRTMIN+2,&act1,NULL);
        sigaction(SIGRTMIN+3,&act2,NULL);
    }
    else {
       /* unblocking appropriate signals */
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask,SIGUSR1);
        sigaddset(&mask,SIGUSR2);
        if (sigprocmask(SIG_UNBLOCK,&mask,NULL) < 0)
            perror("Couldn't unblock signals\n");
        /* setting handlers */
        sigemptyset(&act1.sa_mask);
        sigemptyset(&act2.sa_mask);
        act1.sa_flags = 0;
        act2.sa_flags = SA_SIGINFO;
        act1.sa_sigaction = (void *)handler_1;
        act2.sa_sigaction = (void *)handler_2;        
        sigaction(SIGUSR1,&act1,NULL);
        sigaction(SIGUSR2,&act2,NULL);
    }

    /* catching signals */
    while(catching_in_progress) {
        pause();
    }

    /* sending back signals to sender */
    /* by kill function */
    if (strcmp(argv[1],"kill") == 0) {
        for(int i=0; i<catched_signals; i++)
            kill(sender_pid,SIGUSR1);
        kill(sender_pid,SIGUSR2);
    }
    /* by sigqueue function */
    else if (strcmp(argv[1],"sigqueue") == 0) {
        union sigval s;
        for(int i=0; i<catched_signals; i++) {
            s.sival_int = i;
            sigqueue(sender_pid,SIGUSR1,s);
        }
        s.sival_int = catched_signals;
        sigqueue(sender_pid,SIGUSR2,s);
    }
    /* by kill function but real time signals */
    else if (strcmp(argv[1],"sigrt") == 0) {
         for(int i=0; i<catched_signals; i++)
            kill(sender_pid,SIGRTMIN+2);
        kill(sender_pid,SIGRTMIN+3);
    }

    /* closing message */
    printf("Catcher program catched %d signals\n",catched_signals);

    return 0;
}