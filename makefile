.DEFAULT_GOAL := run

CC?=clang

CFLAGS=-g -I. -I./deps/arena

SOURCES = $(wildcard *.c)
SOURCES+= $(wildcard ./deps/arena/*.c)
OBJECTS=$(SOURCES:%.c=%.o)


%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJECTS) tests

tests: ./t/tests.c $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $< $(OBJECTS)

run: clean $(OBJECTS) tests
	./tests
