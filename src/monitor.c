// src/monitor.c

#include "monitor.h"
#include <stdio.h>
#include <pthread.h>

// Variáveis privadas
static Buffer buffer;
static pthread_mutex_t mutex    = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t not_full  = PTHREAD_COND_INITIALIZER;
static pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

// Controle de métricas
static int produced_count = 0;
static int consumed_count = 0;

// Controle global externo
extern volatile int running;

void monitor_init() {
    buffer.count = 0;
    buffer.in = 0;
    buffer.out = 0;
}

void monitor_destroy() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);
}

void monitor_insert(int item) {
    pthread_mutex_lock(&mutex);
    while (buffer.count == BUFFER_SIZE && running)
        pthread_cond_wait(&not_full, &mutex);

    if (!running) {
        pthread_mutex_unlock(&mutex);
        return;
    }

    buffer.data[buffer.in] = item;
    buffer.in = (buffer.in + 1) % BUFFER_SIZE;
    buffer.count++;
    produced_count++;

    printf("[MON %lu] Inserido: %d | count=%d\n",
           (unsigned long)pthread_self(), item, buffer.count);

    pthread_cond_signal(&not_empty);
    pthread_mutex_unlock(&mutex);
}

int monitor_remove() {
    pthread_mutex_lock(&mutex);
    while (buffer.count == 0 && running)
        pthread_cond_wait(&not_empty, &mutex);

    if (!running && buffer.count == 0) {
        pthread_mutex_unlock(&mutex);
        return -1; // Nada mais para consumir
    }

    int item = buffer.data[buffer.out];
    buffer.out = (buffer.out + 1) % BUFFER_SIZE;
    buffer.count--;
    consumed_count++;

    printf("[MON %lu] Removido: %d | count=%d\n",
           (unsigned long)pthread_self(), item, buffer.count);

    pthread_cond_signal(&not_full);
    pthread_mutex_unlock(&mutex);
    return item;
}

void monitor_print_metrics() {
    printf("\n== EXECUCAO FINALIZADA (MON) ==\n");
    printf("Itens produzidos: %d\n", produced_count);
    printf("Itens consumidos: %d\n", consumed_count);
}

int monitor_get_count() {
    return buffer.count;
}

int monitor_get_capacity() {
    return BUFFER_SIZE;
}

// Função para liberar todas as threads presas
void monitor_wakeup() {
    pthread_mutex_lock(&mutex);
    pthread_cond_broadcast(&not_full);
    pthread_cond_broadcast(&not_empty);
    pthread_mutex_unlock(&mutex);
}
