.PHONY: all clean

CC = gcc
CFLAGS = -Wall -pthread

all: prodcons_nosync prodcons_sem prodcons_mutex prodcons_monitor

# Versão sem sincronização
prodcons_nosync: main_nosync.c buffer.c buffer.h
	$(CC) $(CFLAGS) -o $@ main_nosync.c buffer.c

# Versão semáforo + mutex
prodcons_sem: main.c buffer.c buffer.h
	$(CC) $(CFLAGS) -o $@ main.c buffer.c

# Versão mutex + condição
prodcons_mutex: mutexCond.c buffer.c buffer.h
	$(CC) $(CFLAGS) -o $@ mutexCond.c buffer.c

# Versão monitor simulado
prodcons_monitor: monitorSimulado.c monitor.c monitor.h
	$(CC) $(CFLAGS) -o $@ monitorSimulado.c monitor.c

clean:
	rm -f prodcons_*
