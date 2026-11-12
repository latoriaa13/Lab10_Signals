/**********************************************
 * Filename: signal_alarm.c
 * Author: Ava Latoria
 * Assignment: Lab 10 - Signals
 * Course/Section: CPE-2600 121
 * Description: Schedule and handle a SIGALRM after five seconds
 **********************************************/

#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static volatile sig_atomic_t alarm_received = 0;
static const char alarm_message[] = "Received SIGALRM\n";

// Records and reports delivery of SIGALRM
static void handle_alarm(int signal_number) {
    (void)signal_number;
    alarm_received = 1;
    (void)write(STDOUT_FILENO, alarm_message, sizeof(alarm_message) - 1U);
}

int main(void) {
    struct sigaction action = {0};

    action.sa_handler = handle_alarm;
    if (sigemptyset(&action.sa_mask) == -1 ||
        sigaction(SIGALRM, &action, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    puts("Scheduling an alarm for 5 seconds...");
    fflush(stdout);
    alarm(5);

    while (!alarm_received) {
        pause();
    }

    return EXIT_SUCCESS;
}
