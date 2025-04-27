// src/monitorSimulado.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "monitor.h"
#include "metrics.h"

Metrics metrics;

void* producer(void* arg) {
    while (1) {
        sleep(1);
        int item = rand() % 100;
        monitor_insert(item);
        record_production(&metrics, item);
    }
    return NULL;
}

void* consumer(void* arg) {
    while (1) {
        sleep(2);
        int item = monitor_remove();
        for (int i = 0; i < metrics.total_produced; i++) {
            if (metrics.produced_items[i].item == item) {
                record_consumption(&metrics, item, metrics.produced_items[i].production_time);
                break;
            }
        }
    }
    return NULL;
}

void* monitor_buffer(void* arg) {
    while (1) {
        usleep(100000); // a cada 100ms
        sample_buffer_usage(&metrics, monitor_get_count(), monitor_get_capacity());
    }
    return NULL;
}


int main() {
    srand((unsigned)time(NULL));
    monitor_init();
    init_metrics(&metrics, BUFFER_SIZE);

    int runtime = 15;
    int nprod   = 3;
    int ncons   = 2;
    pthread_t prod[nprod], cons[ncons], monitor_thread;

    for (int i = 0; i < nprod; i++)
        pthread_create(&prod[i], NULL, producer, NULL);
    for (int i = 0; i < ncons; i++)
        pthread_create(&cons[i], NULL, consumer, NULL);


    pthread_create(&monitor_thread, NULL, monitor_buffer, NULL);
    sleep(runtime);
    monitor_print_metrics();
    monitor_destroy();
    calculate_metrics(&metrics, runtime);
    exit(0);
}
