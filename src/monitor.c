#include "monitor.h"
#include <pthread.h>
#include <stdio.h>

// Buffer compartilhado
static Buffer buffer;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
static pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

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

    while (buffer.count == BUFFER_SIZE) {
        pthread_cond_wait(&not_full, &mutex);
    }

    buffer.data[buffer.in] = item;
    buffer.in = (buffer.in + 1) % BUFFER_SIZE;
    buffer.count++;

    printf("[Prod %ld] Inserido: %d | count = %d\n", pthread_self(), item, buffer.count);

    pthread_cond_signal(&not_empty);
    pthread_mutex_unlock(&mutex);
}

int monitor_remove() {
    pthread_mutex_lock(&mutex);

    while (buffer.count == 0) {
        pthread_cond_wait(&not_empty, &mutex);
    }

    int item = buffer.data[buffer.out];
    buffer.out = (buffer.out + 1) % BUFFER_SIZE;
    buffer.count--;

    printf("[Cons %ld] Removido: %d | count = %d\n", pthread_self(), item, buffer.count);

    pthread_cond_signal(&not_full);
    pthread_mutex_unlock(&mutex);

    return item;
}
