//src\monitorSimulado.c

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "monitor.h"

void* producer(void* arg) {
    while (1) {
        sleep(1);
        int item = rand() % 100;
        monitor_insert(item);
    }
    return NULL;
}

void* consumer(void* arg) {
    while (1) {
        sleep(2);
        int item = monitor_remove();
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    monitor_init();

    int qtdProd = 2;
    int qtdCons = 3;
    pthread_t prod[qtdProd], cons[qtdCons];

    for (int i = 0; i < qtdProd; i++) {
        pthread_create(&prod[i], NULL, producer, NULL);
    }

    for (int i = 0; i < qtdCons; i++) {
        pthread_create(&cons[i], NULL, consumer, NULL);
    }

    sleep(10);
    monitor_destroy();
    pthread_exit(NULL);
    return 0;
}
