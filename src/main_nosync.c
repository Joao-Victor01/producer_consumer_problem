// src/main_nosync.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "buffer.h"

int numProd = 2, numCons = 4, buffer_size = 7, runtime_seconds = 10;
int produced_count = 0, consumed_count = 0;
Buffer buffer;

void *producer(void *arg) {
    while (1) {
        sleep(1);
        int item = rand() % 100;
        if (insert_item(&buffer, item) == 0) {
            produced_count++;
            printf("[NOSYNC] Produzido: %d | count=%d\n", item, buffer.count);
        } else {
            printf("[NOSYNC] Buffer cheio, item %d descartado\n", item);
        }
    }
    return NULL;
}

void *consumer(void *arg) {
    while (1) {
        sleep(2);
        int item;
        if (remove_item(&buffer, &item) == 0) {
            consumed_count++;
            printf("[NOSYNC] Consumido: %d | count=%d\n", item, buffer.count);
        } else {
            printf("[NOSYNC] Buffer vazio\n");
        }
    }
    return NULL;
}

int main(int argc, char **argv) {
    srand((unsigned)time(NULL));
    init_buffer(&buffer, buffer_size);

    pthread_t prod[numProd], cons[numCons];
    for (int i = 0; i < numProd; i++)
        pthread_create(&prod[i], NULL, producer, NULL);
    for (int i = 0; i < numCons; i++)
        pthread_create(&cons[i], NULL, consumer, NULL);

    sleep(runtime_seconds);
    printf("\n== EXECUCAO FINALIZADA (NOSYNC) ==\n");
    printf("Itens produzidos: %d\n", produced_count);
    printf("Itens consumidos: %d\n", consumed_count);

    exit(0);
}
