#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <execinfo.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

/* Function to handle given signal and to show siginfo_t structure */
void sgn_handler(int signum, siginfo_t *info, void *secret) {
    /* putting appropriate signal name */
    if(signum == 4) printf("\n\nSignal name = SIGILL\n");
    else if(signum == 29) printf("\n\nSignal name = SIGIO\n");
    else if(signum == 8) printf("\n\nSignal name = SIGFPE\n");
    else if(signum == 11) printf("\n\nSignal name = SIGSEGV\n");
    else if(signum == 20) printf("\n\nSignal name = SIGCHLD\n");
    /* putting shared fields */
    printf("Signal number = %d\n",info->si_signo);
    printf("Sending process PID = %d\n",info->si_pid);
    printf("User sending process UID = %d\n",info->si_uid);
    printf("Signal code = %d",info->si_code);
    if(info->si_code == SI_KERNEL) printf(" (send by kernel)\n");
    else printf(" (send by user)\n");
    printf("Signal error no = %d\n",info->si_errno);
    /* putting unique fields */
    if(signum == 4 || signum == 8 || signum == 11) printf("Address at which fault occurred = %p\n",info->si_addr);
    else if (signum == 29) printf("Band event = %ld\n",info->si_band);
    else if (signum == 20) printf("Child exit code = %d\n",info->si_status);
}

int main(int argc, char *argv[]) {
    struct sigaction action;
    action.sa_handler = (void *) sgn_handler;
    /* emptying mask */
    sigemptyset(&action.sa_mask);
    /* putting SA_SIGINFO flag */
    action.sa_flags = SA_SIGINFO;
    sigaction(atoi(argv[1]),&action,NULL);

    /* raising given signal to process */
    if(atoi(argv[1]) == 20) {
        if(fork() == 0) {
            raise(atoi(argv[1]));
        }
        wait(NULL);
    }
    else
        raise(atoi(argv[1])); 

    return 0;
}