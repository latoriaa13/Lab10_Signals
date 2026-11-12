/**********************************************
 * Filename: tennis_common.h
 * Author: Ava Latoria
 * Assignment: Lab 10 - Signals
 * Course/Section: CPE-2600 121
 * Description: Shared signal protocol and scoring helpers for signal tennis.
 **********************************************/

#ifndef TENNIS_COMMON_H
#define TENNIS_COMMON_H

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

enum {
    POINT_WON_MESSAGE = 0,
    FIRST_EXCHANGE = 1,
    MAX_EXCHANGES = 10,
    MISS_PERCENT = 25
};

struct tennis_score {
    int server_points;
    int receiver_points;
};

static volatile sig_atomic_t tennis_message_ready = 0;
static volatile sig_atomic_t tennis_message_value = 0;
static volatile sig_atomic_t tennis_sender_pid = 0;

// Records the queued value and sender PID; main performs all other work.
static void handle_tennis_signal(int signal_number, siginfo_t *information,
                                 void *context) {
    (void)signal_number;
    (void)context;
    tennis_message_value = information->si_value.sival_int;
    tennis_sender_pid = (sig_atomic_t)information->si_pid;
    tennis_message_ready = 1;
}

// Blocks the ball signal and installs its SA_SIGINFO handler.
static int install_tennis_handler(sigset_t *waiting_mask) {
    struct sigaction action = {0};
    sigset_t blocked_mask;
    int ball_signal = SIGRTMIN;

    if (sigemptyset(&blocked_mask) == -1 ||
        sigaddset(&blocked_mask, ball_signal) == -1 ||
        sigprocmask(SIG_BLOCK, &blocked_mask, waiting_mask) == -1) {
        return -1;
    }

    action.sa_sigaction = handle_tennis_signal;
    action.sa_flags = SA_SIGINFO;
    if (sigemptyset(&action.sa_mask) == -1 ||
        sigaction(ball_signal, &action, NULL) == -1) {
        return -1;
    }

    return 0;
}

// Waits without a race, then returns one queued message and its sender.
static void wait_for_tennis_message(const sigset_t *waiting_mask,
                                    pid_t *sender, int *value) {
    while (!tennis_message_ready) {
        (void)sigsuspend(waiting_mask);
    }

    *sender = (pid_t)tennis_sender_pid;
    *value = (int)tennis_message_value;
    tennis_message_ready = 0;
}

// Sends one reliably queued real-time signal message.
static int send_tennis_message(pid_t destination, int value) {
    union sigval queued_value;

    queued_value.sival_int = value;
    return sigqueue(destination, SIGRTMIN, queued_value);
}

// Waits a random 1–2 seconds; TENNIS_FAST shortens automated tests only.
static void tennis_delay(void) {
    struct timespec delay;
    struct timespec remaining;
    const char *fast_mode = getenv("TENNIS_FAST");
    long milliseconds;

    if (fast_mode != NULL && fast_mode[0] != '\0') {
        milliseconds = 10L + (rand() % 11);
    } else {
        milliseconds = 1000L + (rand() % 1001);
    }

    delay.tv_sec = milliseconds / 1000L;
    delay.tv_nsec = (milliseconds % 1000L) * 1000000L;
    while (nanosleep(&delay, &remaining) == -1 && errno == EINTR) {
        delay = remaining;
    }
}

// Returns true when a player has won a traditional tennis game.
static int tennis_game_won(int player_points, int opponent_points) {
    return player_points >= 4 && player_points - opponent_points >= 2;
}

// Prints traditional tennis scoring from a shared server/receiver view.
static void print_tennis_score(const struct tennis_score *score) {
    static const char *const names[] = {"Love", "15", "30", "40"};
    int server = score->server_points;
    int receiver = score->receiver_points;

    if (tennis_game_won(server, receiver)) {
        puts("Score: Game, Server");
    } else if (tennis_game_won(receiver, server)) {
        puts("Score: Game, Receiver");
    } else if (server >= 3 && receiver >= 3) {
        if (server == receiver) {
            puts("Score: Deuce");
        } else if (server > receiver) {
            puts("Score: Advantage Server");
        } else {
            puts("Score: Advantage Receiver");
        }
    } else {
        printf("Score: Server %s - Receiver %s\n", names[server],
               names[receiver]);
    }
    fflush(stdout);
}

// Converts and validates a positive PID supplied on the command line.
static inline int parse_tennis_pid(const char *text, pid_t *result) {
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

#endif
