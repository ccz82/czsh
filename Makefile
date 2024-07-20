CC = gcc
CSTD = c99
CFLAGS = -std=$(CSTD)

all: czsh

czsh: main.c builtins.c
	$(CC) -o $@ $^ $(CFLAGS)

run:
	./czsh

clean:
	rm czsh

.PHONY: clean
