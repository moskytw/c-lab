# commands

CC = gcc
CFLAGS = -O3 -ansi -I src/lib

# targets

.PHONY: default
default: bin obj $(foreach PATH,$(wildcard src/bin/*.c),$(patsubst src/bin/%.c,bin/%,$(PATH)))

bin:
	mkdir bin

obj:
	mkdir obj

bin/%: src/bin/%.c $(foreach PATH,$(wildcard src/lib/*.c),$(patsubst src/lib/%.c,obj/%.o,$(PATH)))
	$(CC) $(CFLAGS) $^ -o $@

.PRECIOUS: obj/%.o
obj/%.o: src/lib/%.c
	$(CC) $(CFLAGS) -c src/lib/$*.c -o obj/$*.o

.PHONY: clean
clean:
	rm -r bin obj


