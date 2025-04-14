CC = gcc
CFLAGS = -Wall -pthread

all: main

main: main.c buffer.c
	$(CC) $(CFLAGS) -o main main.c buffer.c

clean:
	rm -f main
