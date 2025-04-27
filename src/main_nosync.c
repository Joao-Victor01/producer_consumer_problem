// src/main_nosync.c
// -----------------
// Implementação 'nosync' do problema Produtor-Consumidor:
// Dois produtores e quatro consumidores interagem em um buffer circular de tamanho fixo.
// Não há mecanismos de sincronização: produtores podem descartar itens se o buffer estiver cheio,
// consumidores podem falhar em remover se o buffer estiver vazio.
// Métricas de produção, consumo, latência e utilização do buffer são coletadas.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>   // Threads POSIX
#include <unistd.h>    // sleep, usleep
#include <time.h>      // srand, time
#include "buffer.h"    // Buffer circular de inteiros
#include "metrics.h"   // Coleta e cálculo de métricas

// Parâmetros globais (podem ser configurados via CLI):
int numProd = 2;           // qtd de threads produtoras
int numCons = 4;           // qtd de threads consumidoras
int buffer_size = 7;       // Capacidade do buffer circular
int runtime_seconds = 10;  // Duração da execução em segundos

// Contadores globais de itens produzidos/consumidos:
int produced_count = 0;
int consumed_count = 0;

// Estruturas de dados compartilhadas:
Buffer buffer;    // Buffer circular (sem sincronização)
Metrics metrics;  // Armazena dados para cálculo de métricas


// Função executada pelas threads produtoras:
void *producer(void *arg) {
    while (1) {
        sleep(1);  
        int item = rand() % 100; 

        // Tenta inserir no buffer; se cheio, descarta o item
        if (insert_item(&buffer, item) == 0) {
            produced_count++;
            record_production(&metrics, item);  // Registra métrica de produção
            printf("[NOSYNC] Produzido: %d | count=%d\n", item, buffer.count);
        } else {
            printf("[NOSYNC] Buffer cheio, item %d descartado\n", item);
        }
    }
    return NULL;
}


// Função executada pelas threads consumidoras:
void *consumer(void *arg) {
    while (1) {
        sleep(2);  
        int item;

        // Tenta remover do buffer; se vazio, informa erro
        if (remove_item(&buffer, &item) == 0) {
            consumed_count++;
            // Encontra o timestamp de produção para calcular latência
            for (int i = 0; i < metrics.total_produced; i++) {
                if (metrics.produced_items[i].item == item) {
                    record_consumption(&metrics, item, metrics.produced_items[i].production_time);
                    break;
                }
            }
            printf("[NOSYNC] Consumido: %d | count=%d\n", item, buffer.count);
        } else {
            printf("[NOSYNC] Buffer vazio\n");
        }
    }
    return NULL;
}


// Thread de monitoramento do buffer para métricas de utilização:
void *monitor_buffer(void *param) {
    while (1) {
        usleep(100000);  // Amostragem a cada 100ms
        sample_buffer_usage(&metrics, buffer.count, buffer.capacity);
    }
    return NULL;   
}


int main(int argc, char **argv) {
    srand((unsigned)time(NULL));

    // Configura buffer e métricas com a capacidade definida
    init_buffer(&buffer, buffer_size);
    init_metrics(&metrics, buffer_size);

    // Cria threads produtoras e consumidoras
    pthread_t prod[numProd], cons[numCons], monitor_thread;
    for (int i = 0; i < numProd; i++)
        pthread_create(&prod[i], NULL, producer, NULL);
    for (int i = 0; i < numCons; i++)
        pthread_create(&cons[i], NULL, consumer, NULL);

    // Inicia thread de monitoramento do buffer
    pthread_create(&monitor_thread, NULL, monitor_buffer, NULL);

    // Mantém o programa rodando pelo tempo definido
    sleep(runtime_seconds);

    // Ao terminar, exibe contagens e calcula métricas finais
    printf("\n== EXECUCAO FINALIZADA (NOSYNC) ==\n");
    printf("Itens produzidos: %d\n", produced_count);
    printf("Itens consumidos: %d\n", consumed_count);

    calculate_metrics(&metrics, runtime_seconds);
    
    // mata todas as threads
    exit(0);
}
