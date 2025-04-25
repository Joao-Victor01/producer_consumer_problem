#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "buffer.h"
#include <stdlib.h>

Buffer buffer;

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

        printf("[Prod %ld] Antes: rear=%d, count=%d\n", pthread_self(), buffer.rear, buffer.count);

        if(!insert_item(&buffer, item)) { // Tenta inserir o item no buffer
            printf("Produzido: %d\n", item);
        } else {
            printf("Buffer cheio, nao foi possivel produzir: %d\n", item);
        }
        printf("[Prod %ld] Depois: rear=%d, count=%d\n", pthread_self(), buffer.rear, buffer.count);
    }
}

void* consumer (void *param) {
    int item;
    while (1) {
        sleep(2.1); // Simula o tempo de consumo

        printf("[Cons %ld] Antes: front=%d, count=%d\n", pthread_self(), buffer.front, buffer.count);
        
        if(!remove_item(&buffer, &item)) { // Tenta remover um item do buffer
            printf("Consumido: %d\n", item);
        } else {
            printf("Buffer vazio, nao foi possivel consumir\n");
        }
        printf("[Cons %ld] Depois: front=%d, count=%d\n", pthread_self(), buffer.front, buffer.count);
    }
}

int main() {
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