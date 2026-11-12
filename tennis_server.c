/**********************************************
 * Filename: tennis_server.c
 * Author: Ava Latoria
 * Assignment: Lab 10 - Signals
 * Course/Section: CPE-2600 121
 * Description: Serves and plays the signal-tennis game.
 **********************************************/

#define _POSIX_C_SOURCE 200809L

#include "tennis_common.h"

// Waits briefly and serves the first exchange of a new point.
static int serve_ball(pid_t receiver_pid) {
    tennis_delay();
    printf("\aServer serves exchange 1 to PID %ld.\n", (long)receiver_pid);
    fflush(stdout);
    return send_tennis_message(receiver_pid, FIRST_EXCHANGE);
}

int main(int argc, char *argv[]) {
    struct tennis_score score = {0, 0};
    sigset_t waiting_mask;
    pid_t receiver_pid;

    if (argc != 2 || parse_tennis_pid(argv[1], &receiver_pid) == -1) {
        fprintf(stderr, "Usage: %s <receiver-pid>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (install_tennis_handler(&waiting_mask) == -1) {
        perror("install_tennis_handler");
        return EXIT_FAILURE;
    }

    srand((unsigned int)(time(NULL) ^ (time_t)getpid()));
    printf("Server ready. PID: %ld; receiver PID: %ld\n", (long)getpid(),
           (long)receiver_pid);
    fflush(stdout);

    if (serve_ball(receiver_pid) == -1) {
        perror("sigqueue");
        return EXIT_FAILURE;
    }

    while (1) {
        pid_t sender;
        int message;
        int missed_return;

        wait_for_tennis_message(&waiting_mask, &sender, &message);
        if (sender != receiver_pid) {
            fprintf(stderr, "Ignoring signal from unexpected PID %ld\n",
                    (long)sender);
            continue;
        }

        if (message == POINT_WON_MESSAGE) {
            puts("Server wins the point: the receiver missed.");
            score.server_points++;
            print_tennis_score(&score);
            if (tennis_game_won(score.server_points,
                                score.receiver_points)) {
                puts("Server wins the game!");
                return EXIT_SUCCESS;
            }
            if (serve_ball(receiver_pid) == -1) {
                perror("sigqueue");
                return EXIT_FAILURE;
            }
            continue;
        }

        if (message < FIRST_EXCHANGE || message > MAX_EXCHANGES) {
            fprintf(stderr, "Ignoring invalid tennis message %d\n", message);
            continue;
        }

        printf("Server received exchange %d from PID %ld.\n", message,
               (long)sender);
        fflush(stdout);
        tennis_delay();

        missed_return = message == MAX_EXCHANGES ||
                        rand() % 100 < MISS_PERCENT;
        if (missed_return) {
            if (message == MAX_EXCHANGES) {
                puts("Ten-exchange volley complete; server misses next.");
            } else {
                puts("Server randomly misses the return.");
            }
            puts("Receiver wins the point.");
            score.receiver_points++;
            print_tennis_score(&score);
            if (send_tennis_message(receiver_pid, POINT_WON_MESSAGE) == -1) {
                perror("sigqueue");
                return EXIT_FAILURE;
            }
            if (tennis_game_won(score.receiver_points,
                                score.server_points)) {
                puts("Receiver wins the game!");
                return EXIT_SUCCESS;
            }
            if (serve_ball(receiver_pid) == -1) {
                perror("sigqueue");
                return EXIT_FAILURE;
            }
            continue;
        }

        printf("\aServer hits exchange %d back to the receiver.\n",
               message + 1);
        fflush(stdout);
        if (send_tennis_message(receiver_pid, message + 1) == -1) {
            perror("sigqueue");
            return EXIT_FAILURE;
        }
    }
}
