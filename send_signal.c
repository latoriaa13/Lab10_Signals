/**********************************************
 * Filename: send_signal.c
 * Author: Ava Latoria
 * Assignment: Lab 10 - Signals
 * Course/Section: CPE-2600 121
 * Description: Sends a random integer with SIGUSR1
 **********************************************/

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Converts and validates the receiver PID from the command line
static int parse_pid(const char *text, pid_t *result) {
    char *end = NULL;
    long value;

    errno = 0;
    value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value <= 0 ||
        value > INT_MAX) {
        return -1;
    }

    *result = (pid_t)value;
    return 0;
}

int main(int argc, char *argv[]) {
    pid_t receiver_pid;
    union sigval value;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <receiver-pid>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (parse_pid(argv[1], &receiver_pid) == -1) {
        fprintf(stderr, "Invalid receiver PID: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    srand((unsigned int)(time(NULL) ^ (time_t)getpid()));
    value.sival_int = rand();
    printf("Sending integer %d to process %ld\n", value.sival_int,
           (long)receiver_pid);

    if (sigqueue(receiver_pid, SIGUSR1, value) == -1) {
        perror("sigqueue");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
