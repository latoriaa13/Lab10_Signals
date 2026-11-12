# CPE-2600 Lab 10: Signals

This repository contains the required programs for Lab 10. They demonstrate
installing signal handlers, receiving operating-system signals, obtaining
signal metadata with `sigaction`, and sending an integer with `sigqueue`.

## Build

```sh
make
```

The Makefile builds these programs:

- `signal_handler` handles SIGINT and continues running.
- `signal_alarm` schedules and handles SIGALRM after five seconds.
- `signal_segfault` deliberately triggers and handles SIGSEGV.
- `signal_sigaction` reports the PID that sent SIGUSR1.
- `recv_signal` receives an integer sent with SIGUSR1.
- `send_signal` sends a random integer to `recv_signal` with SIGUSR1.
- `tennis_receiver` receives the first serve in the extra-credit game.
- `tennis_server` serves and plays the extra-credit game.

Run `make clean` to remove all generated executables.

## Run

For the SIGINT example:

```sh
./signal_handler
# Press Ctrl+C, or run this in another terminal:
kill -SIGINT <printed-pid>
```

The handler returns, so the process continues. Use `kill -SIGKILL
<printed-pid>` from another terminal when finished.

For the alarm and segmentation-fault examples:

```sh
./signal_alarm
timeout 1 ./signal_segfault
```

`signal_segfault` intentionally repeats forever because returning from its
handler retries the same invalid memory access. Use `timeout` or SIGKILL to
stop it.

For the `sigaction` example, start the program and send SIGUSR1 from a second
terminal:

```sh
./signal_sigaction
kill -SIGUSR1 <printed-pid>
```

For the queued-data example, start the receiver first. Pass its printed PID to
the sender from a second terminal:

```sh
./recv_signal
./send_signal <receiver-pid>
```

The sender and receiver print the same random integer. Stop either waiting
program with Ctrl+C when the demonstration is complete.

## Extra Credit: Signal Tennis

The tennis programs use queued real-time signals so every ball and scoring
message is delivered. Each return waits a random one to two seconds, rings the
system bell, and has a 25% chance of missing. A rally is capped at 10 exchanges.
The server serves again after every point, and both programs exit cleanly when
one player wins a traditional tennis game (four points and a two-point lead).

Start the receiver first:

```sh
./tennis_receiver
```

Then pass its printed PID to the server in another terminal:

```sh
./tennis_server <receiver-pid>
```
