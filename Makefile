# commands

CC = gcc
CFLAGS = -O3 -Wall -I src/lib

# targets

.PHONY: default
default: $(foreach PATH,$(wildcard src/bin/*.c),$(patsubst src/bin/%.c,bin/%,$(PATH)))

bin/test_alarm: src/bin/test_alarm.c obj/termios_util.o
bin/test_fcntl: src/bin/test_fcntl.c obj/termios_util.o
bin/test_fcntl_async: src/bin/test_fcntl_async.c obj/termios_util.o
bin/test_fork: src/bin/test_fork.c obj/termios_util.o
bin/test_pthread: src/bin/test_pthread.c obj/termios_util.o
bin/test_ansi_esc_seq: src/bin/test_ansi_esc_seq.c obj/ansi_esc_seq.o
bin/test_termios_util: src/bin/test_termios_util.c obj/termios_util.o

bin/%:
	$(CC) $(CFLAGS) $^ -o $@

.PRECIOUS: obj/%.o
obj/%.o: src/lib/%.c
	$(CC) $(CFLAGS) -c src/lib/$*.c -o obj/$*.o

.PHONY: clean
clean:
	rm bin/* obj/*


