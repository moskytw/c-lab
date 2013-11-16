## The commands: ##

CC = gcc
CFLAGS = -O3 -Wall -I src/lib

## The targets: ##

# By default, it scans the all source files (src/bin/%.c) and require them to be
# built into binary files (bin/%).
.PHONY: all
all: $(foreach PATH,$(wildcard src/bin/*.c),$(patsubst src/bin/%.c,bin/%,$(PATH)))

# Each binary needs the corresponding source file.
bin/%: src/bin/%.c
	$(CC) $(CFLAGS) $^ -o $@

# Define additional dependences of binary files:
bin/test_ansi_esc_seq: obj/ansi_esc_seq.o
bin/test_termios_util bin/test_fcntl bin/test_fcntl_async bin/test_alarm bin/test_fork bin/test_pthread: obj/termios_util.o

# Each object needs corresponding source file.
obj/%.o: src/lib/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	rm bin/* obj/*


