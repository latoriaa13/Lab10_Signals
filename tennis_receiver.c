/**********************************************
 * Filename: tennis_receiver.c
 * Author: Ava Latoria
 * Assignment: Lab 10 - Signals
 * Course/Section: CPE-2600 121
 * Description: Receives the first serve in signal-tennis.
 **********************************************/

#define _POSIX_C_SOURCE 200809L

#include "tennis_common.h"

int main(void) {
    struct tennis_score score = {0, 0};
    sigset_t waiting_mask;
    pid_t server_pid = 0;

    if (install_tennis_handler(&waiting_mask) == -1) {
        perror("install_tennis_handler");
        return EXIT_FAILURE;
    }

    srand((unsigned int)(time(NULL) ^ (time_t)getpid()));
    printf("Receiver ready. PID: %ld\n", (long)getpid());
    puts("Start the server with: ./tennis_server <receiver-pid>");
    fflush(stdout);

    while (1) {
        pid_t sender;
        int message;
        int missed_return;

        wait_for_tennis_message(&waiting_mask, &sender, &message);
        if (server_pid == 0) {
            server_pid = sender;
        } else if (sender != server_pid) {
            fprintf(stderr, "Ignoring signal from unexpected PID %ld\n",
                    (long)sender);
            continue;
        }

        if (message == POINT_WON_MESSAGE) {
            puts("Receiver wins the point: the server missed.");
            score.receiver_points++;
            print_tennis_score(&score);
            if (tennis_game_won(score.receiver_points,
                                score.server_points)) {
                puts("Receiver wins the game!");
                return EXIT_SUCCESS;
            }
            continue;
        }

        if (message < FIRST_EXCHANGE || message > MAX_EXCHANGES) {
            fprintf(stderr, "Ignoring invalid tennis message %d\n", message);
            continue;
        }

        printf("Receiver received exchange %d from PID %ld.\n", message,
               (long)sender);
        fflush(stdout);
        tennis_delay();

        missed_return = message == MAX_EXCHANGES ||
                        rand() % 100 < MISS_PERCENT;
        if (missed_return) {
            if (message == MAX_EXCHANGES) {
                puts("Ten-exchange volley complete; receiver misses next.");
            } else {
                puts("Receiver randomly misses the return.");
            }
            puts("Server wins the point.");
            score.server_points++;
            print_tennis_score(&score);
            if (send_tennis_message(server_pid, POINT_WON_MESSAGE) == -1) {
                perror("sigqueue");
                return EXIT_FAILURE;
            }
            if (tennis_game_won(score.server_points,
                                score.receiver_points)) {
                puts("Server wins the game!");
                return EXIT_SUCCESS;
            }
            continue;
        }

        printf("\aReceiver hits exchange %d back to the server.\n",
               message + 1);
        fflush(stdout);
        if (send_tennis_message(server_pid, message + 1) == -1) {
            perror("sigqueue");
            return EXIT_FAILURE;
        }
    }
}
