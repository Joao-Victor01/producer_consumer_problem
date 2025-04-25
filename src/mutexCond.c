// src/mutexCond.c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "buffer.h"
#include <stdlib.h>
#include <time.h>

Buffer buffer;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

void* producer (void *param) {
    int item;
    while (1) {
        sleep(1); // Simula o tempo de produção
        item = rand() % 100;

        pthread_mutex_lock(&mutex);
        while (buffer.count == buffer.capacity) {
            pthread_cond_wait(&not_full, &mutex);
        }

        insert_item(&buffer, item);
        printf("[Prod %lu] Inserido: %d | count = %d\n", (unsigned long)pthread_self(), item, buffer.count);

        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* consumer (void *param) {
    int item;
    while (1) {
        sleep(2); // Simula o tempo de consumo

        pthread_mutex_lock(&mutex);
        while (buffer.count == 0) {
            pthread_cond_wait(&not_empty, &mutex);
        }

        remove_item(&buffer, &item);
        printf("[Cons %lu] Removido: %d | count = %d\n", (unsigned long)pthread_self(), item, buffer.count);

        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    srand((unsigned int)time(NULL));

    if (init_buffer(&buffer, 5) != 0) {
        printf("Erro ao inicializar o buffer\n");
        exit(1);
    }

    int mainSleepTime = 10;
    int qtdProdutores = 2;
    int qtdConsumidores = 4;

    pthread_t producers[qtdProdutores];
    pthread_t consumers[qtdConsumidores];

    for (int i = 0; i < qtdProdutores; i++) {
        pthread_create(&producers[i], NULL, producer, NULL);
    }
    for (int i = 0; i < qtdConsumidores; i++) {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }

    sleep(mainSleepTime);

    printf("\n== Execucao Finalizada (mutexCond) ==\n");
    return 0;
}
