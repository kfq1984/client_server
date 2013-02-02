CC=gcc
CFLAGS=-Wall -g

all: client server

HEADERS = $(wildcard *.h)

client: client.o
	$(CC) $(CFLAGS) -o client client.o

server: server.o
	$(CC) $(CFLAGS) -o server server.o

.PHONY: clean

clean:
	rm -f client server
