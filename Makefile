## The commands: ##

CC = gcc
CFLAGS = -O3 -Wall -I src/lib

## The targets: ##

.PHONY: default
# By default, we scan the all source files (src/bin/%.c) and require them to be
# built into binary (bin/%).
default: $(foreach PATH,$(wildcard src/bin/*.c),$(patsubst src/bin/%.c,bin/%,$(PATH)))

# Define the dependences:
#
bin/test_ansi_esc_seq: src/bin/test_ansi_esc_seq.c obj/ansi_esc_seq.o
bin/test_termios_util: src/bin/test_termios_util.c obj/termios_util.o

bin/test_alarm: src/bin/test_alarm.c obj/termios_util.o
bin/test_fcntl: src/bin/test_fcntl.c obj/termios_util.o
bin/test_fcntl_async: src/bin/test_fcntl_async.c obj/termios_util.o
bin/test_fork: src/bin/test_fork.c obj/termios_util.o
bin/test_pthread: src/bin/test_pthread.c obj/termios_util.o

bin/test_socket: src/bin/test_socket.c

# It is a shortcut for the above targets.
bin/%:
	$(CC) $(CFLAGS) $^ -o $@

# Define how to build the object files.
obj/%.o: src/lib/%.c
	$(CC) $(CFLAGS) -c src/lib/$*.c -o obj/$*.o

# Don't remove the object files. They can be reused for next build.
.PRECIOUS: obj/%.o

.PHONY: clean
clean:
	rm bin/* obj/*


