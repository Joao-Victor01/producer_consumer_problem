// src/monitorSimulado.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "monitor.h"
#include "metrics.h"

Metrics metrics;
volatile int running = 1; // Controle global

void* producer(void* arg) {
    while (running) {
        sleep(1);
        int item = rand() % 100;
        monitor_insert(item);
        record_production(&metrics, item);
    }
    return NULL;
}

void* consumer(void* arg) {
    while (running) {
        sleep(2);
        int item = monitor_remove();
        if (item == -1) break; // Se monitor_remove() indicar fim, sai da thread

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
    while (running) {
        usleep(100000); // Coleta uso de buffer a cada 100ms
        sample_buffer_usage(&metrics, monitor_get_count(), monitor_get_capacity());
    }
    return NULL;
}

int main() {
    srand((unsigned)time(NULL));

    monitor_init();
    init_metrics(&metrics, BUFFER_SIZE);

    int runtime = 15;
    int nprod = 3;
    int ncons = 2;
    pthread_t prod[nprod], cons[ncons], monitor_thread;

    // Cria as threads
    for (int i = 0; i < nprod; i++)
        pthread_create(&prod[i], NULL, producer, NULL);
    for (int i = 0; i < ncons; i++)
        pthread_create(&cons[i], NULL, consumer, NULL);
    pthread_create(&monitor_thread, NULL, monitor_buffer, NULL);

    // Roda por runtime segundos
    sleep(runtime);

    // Sinaliza para parar
    running = 0;
    monitor_wakeup(); // Libera threads bloqueadas

    // Aguarda todas as threads
    for (int i = 0; i < nprod; i++)
        pthread_join(prod[i], NULL);
    for (int i = 0; i < ncons; i++)
        pthread_join(cons[i], NULL);
    pthread_join(monitor_thread, NULL);

    // Imprime métricas
    monitor_print_metrics();
    monitor_destroy();
    calculate_metrics(&metrics, runtime);

    return 0;
}
