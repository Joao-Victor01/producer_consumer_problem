// src/monitorSimulado.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
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
        monitor_remove();
    }
    return NULL;
}

int main() {
    srand((unsigned)time(NULL));
    monitor_init();

    int runtime = 15;
    int nprod   = 3;
    int ncons   = 2;
    pthread_t prod[nprod], cons[ncons];

    for (int i = 0; i < nprod; i++)
        pthread_create(&prod[i], NULL, producer, NULL);
    for (int i = 0; i < ncons; i++)
        pthread_create(&cons[i], NULL, consumer, NULL);

    sleep(runtime);
    monitor_print_metrics();
    monitor_destroy();
    exit(0);
}
