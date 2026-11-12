/**********************************************
 * Filename: signal_segfault.c
 * Author: Ava Latoria
 * Assignment: Lab 10 - Signals
 * Course/Section: CPE-2600 121
 * Description: Demonstrate handling of SIGSEGV
 **********************************************/

#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static const char segfault_message[] =
    "Received a segmentation fault (SIGSEGV)\n";

// Reports SIGSEGV and returns, as required by the experiment
static void handle_segfault(int signal_number) {
    (void)signal_number;
    (void)write(STDOUT_FILENO, segfault_message,
                sizeof(segfault_message) - 1U);
}

int main(void) {
    int *value = NULL;
    struct sigaction action = {0};

    action.sa_handler = handle_segfault;
    if (sigemptyset(&action.sa_mask) == -1 ||
        sigaction(SIGSEGV, &action, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    //Deliberately faults. The handler returns and this line is retried
    printf("The value is: %d\n", *value);

    return EXIT_SUCCESS;
}
