/**********************************************
 * Filename: signal_sigaction.c
 * Author: Ava Latoria
 * Assignment: Lab 10 - Signals
 * Course/Section: CPE-2600 121
 * Description: Uses sigaction to handle SIGUSR1 and print PID
 **********************************************/

#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Prints the sender details supplied with SIGUSR1
static void handle_user_signal(int signal_number, siginfo_t *information,
                               void *context) {
    (void)signal_number;
    (void)context;
    printf("SIGUSR1 sent by process %ld\n", (long)information->si_pid);
    fflush(stdout);
}

int main(void) {
    struct sigaction action = {0};

    action.sa_sigaction = handle_user_signal;
    action.sa_flags = SA_SIGINFO;
    if (sigemptyset(&action.sa_mask) == -1 ||
        sigaction(SIGUSR1, &action, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    printf("Waiting for SIGUSR1. PID: %ld\n", (long)getpid());
    fflush(stdout);

    while (1) {
        pause();
    }
}
