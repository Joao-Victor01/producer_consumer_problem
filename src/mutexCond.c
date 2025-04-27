// src/mutexCond.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "buffer.h"
#include "metrics.h"

Buffer buffer;
pthread_mutex_t mutex    = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full  = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
Metrics metrics;

// métricas
int produced_count = 0;
int consumed_count = 0;

void* producer(void *param) {
    int item;
    while (1) {
        sleep(1);
        item = rand() % 100;

        pthread_mutex_lock(&mutex);
        while (buffer.count == buffer.capacity)
            pthread_cond_wait(&not_full, &mutex);

        insert_item(&buffer, item);
        produced_count++;
        record_production(&metrics, item);
        printf("[MUTEX %lu] Inserido: %d | count=%d\n",
               (unsigned long)pthread_self(), item, buffer.count);

        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* consumer(void *param) {
    int item;
    while (1) {
        sleep(2);

        pthread_mutex_lock(&mutex);
        while (buffer.count == 0)
            pthread_cond_wait(&not_empty, &mutex);

        remove_item(&buffer, &item);
        consumed_count++;
        for (int i = 0; i < metrics.total_produced; i++) {
            if (metrics.produced_items[i].item == item) {
                record_consumption(&metrics, item, metrics.produced_items[i].production_time);
                break;
            }
        }
        printf("[MUTEX %lu] Removido: %d | count=%d\n",
               (unsigned long)pthread_self(), item, buffer.count);   

        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *monitor_buffer(void *param) {
    while (1) {
        usleep(100000); // amostra a cada 100ms
        pthread_mutex_lock(&mutex);
        sample_buffer_usage(&metrics, buffer.count, buffer.capacity);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;   
}

int main() {
    srand((unsigned)time(NULL));
    if (init_buffer(&buffer, 5) != 0) {
        fprintf(stderr, "Erro ao inicializar buffer\n");
        exit(1);
    }
    init_metrics(&metrics, 5);

    int runtime = 10;
    int nprod   = 2;
    int ncons   = 4;

    pthread_t prod[nprod], cons[ncons], monitor_thread;
    for (int i = 0; i < nprod; i++)
        pthread_create(&prod[i], NULL, producer, NULL);
    for (int i = 0; i < ncons; i++)
        pthread_create(&cons[i], NULL, consumer, NULL);

    pthread_create(&monitor_thread, NULL, monitor_buffer, NULL);   
    sleep(runtime);


    printf("\n== EXECUÇÃO FINALIZADA (MUTEX) ==\n");
    printf("Itens produzidos: %d\n", produced_count);
    printf("Itens consumidos: %d\n", consumed_count);

    calculate_metrics(&metrics, runtime);
    exit(0);
}
