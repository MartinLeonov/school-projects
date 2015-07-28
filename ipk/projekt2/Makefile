CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror -W -pedantic
RM=rm -f

all: server client

server.o: server.c server.h
	$(CC) $(CFLAGS) -c server.c -o $@

server: server.o
	$(CC) server.o -o $@

client.o: client.c client.h
	$(CC) $(CFLAGS) -c client.c -o $@

client: client.o
	$(CC) client.o -o $@

.PHONY: clean
clean:
	$(RM) *.o server client
