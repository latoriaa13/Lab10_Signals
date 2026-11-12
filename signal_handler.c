/**********************************************
 * Filename: signal_handler.c
 * Author: Ava Latoria
 * Assignment: Lab 10 - Signals
 * Course/Section: CPE-2600 121
 * Description: Handle SIGINT without terminating the process
 **********************************************/

#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static const char signal_message[] = "Received SIGINT; continuing execution\n";

// Handles SIGINT and returns control to the interrupted code
static void handle_signal(int signal_number) {
    (void)signal_number;
    (void)write(STDOUT_FILENO, signal_message, sizeof(signal_message) - 1U);
}

int main(void) {
    struct sigaction action = {0};

    action.sa_handler = handle_signal;
    if (sigemptyset(&action.sa_mask) == -1 ||
        sigaction(SIGINT, &action, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    printf("PID: %ld\n", (long)getpid());
    printf("Send SIGINT with Ctrl+C or: kill -SIGINT %ld\n", (long)getpid());
    fflush(stdout);

    while (1) {
        puts("Sleeping");
        sleep(1);
    }
}
