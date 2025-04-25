//src\mutexCond.c

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "buffer.h"
#include <stdlib.h>

Buffer buffer;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

void* imprimir_ola(void* arg) {
    int id = *((int*)arg);
    while(1){
        sleep(id);
        printf("Olá da thread!%d\n",id);
    }
    return NULL;
}

void* producer (void *param) {
    int item;
    while (1) {
        sleep(1); // Simula o tempo de produção
        item = rand() % 100; // Produz um item aleatório

        pthread_mutex_lock(&mutex);

        // Espera enquanto o buffer estiver cheio
        while (buffer.count == buffer.capacity) {
            pthread_cond_wait(&not_full, &mutex);
        }

        insert_item(&buffer, item);
        printf("[Prod %ld] Inserido: %d | count = %d\n", pthread_self(), item, buffer.count);

        pthread_cond_signal(&not_empty); // Avisa que tem item
        pthread_mutex_unlock(&mutex);
    }
}

void* consumer (void *param) {
    int item;
    while (1) {
        sleep(2); // Simula o tempo de consumo

        pthread_mutex_lock(&mutex);

        // Espera enquanto o buffer estiver vazio
        while (buffer.count == 0) {
            pthread_cond_wait(&not_empty, &mutex);
        }

        remove_item(&buffer, &item);
        printf("[Cons %ld] Removido: %d | count = %d\n", pthread_self(), item, buffer.count);

        pthread_cond_signal(&not_full); // Avisa que tem espaço
        pthread_mutex_unlock(&mutex);
    }
}

int main() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);
    srand((unsigned int)time(NULL)); // Inicializa o gerador de números aleatórios

    if (init_buffer(&buffer, 5) != 0) {
        printf("Erro ao inicializar o buffer\n");
        exit(1);
    }

    int mainSleepTime = 10; // Tempo total de execução do programa
    int qtdProdutores = 2; // Quantidade de threads produtoras
    int qtdConsumidores = 4; // Quantidade de threads consumidoras

    pthread_t producers[qtdProdutores];
    pthread_t consumers[qtdConsumidores];

    for(int i = 0; i < qtdProdutores; i++) {
        pthread_create(&producers[i], NULL, producer, NULL);
    }
    for(int i = 0; i < qtdConsumidores; i++) {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }

    sleep(mainSleepTime); // Tempo total de execução do programa
    pthread_exit(NULL); // Finaliza todas as threads
    exit(0); // Encerra o programa
}