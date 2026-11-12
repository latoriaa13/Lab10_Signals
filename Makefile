CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -g

PROGRAMS := signal_handler signal_alarm signal_segfault signal_sigaction \
	recv_signal send_signal tennis_receiver tennis_server

.PHONY: all clean

all: $(PROGRAMS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

tennis_receiver tennis_server: tennis_common.h

clean:
	$(RM) $(PROGRAMS) *.o
