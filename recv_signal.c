/**********************************************
 * Filename: recv_signal.c
 * Author: Ava Latoria
 * Assignment: Lab 10 - Signals
 * Course/Section: CPE-2600 121
 * Description: Receive and print an integer sent with SIGUSR1
 **********************************************/

#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Retrieves the queued integer from SIGUSR1 metadata
static void handle_user_signal(int signal_number, siginfo_t *information,
                               void *context) {
    (void)signal_number;
    (void)context;
    printf("Received integer: %d\n", information->si_value.sival_int);
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

    printf("Receiver PID: %ld\n", (long)getpid());
    fflush(stdout);

    while (1) {
        pause();
    }
}
