CC = gcc

CFLAGS = -Wall -Wextra -Werror

sources = $(wildcard src/*.c)
objects = $(patsubst src/%.c,bin/%.o,$(sources))
out = bin/cb

bin/%.o : src/%.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(out) : $(objects)
	$(CC) $(CFLAGS) -o $(out) $(objects)
