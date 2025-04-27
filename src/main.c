/**
 * src/main.c
 * Producer-Consumer Problem — Versão SEMÁFORO + MUTEX
 *
 * Implementa um buffer circular compartilhado entre múltiplas threads
 * produtoras e consumidoras, utilizando POSIX semáforos para contar
 * vagas (empty) e itens (full), e um mutex para exclusão mútua.
 *
 * Cada produtor gera um item a cada 1s, cada consumidor consome a cada 2s.
 * Parâmetros configuráveis via linha de comando:
 *   --prod N      número de produtores
 *   --cons M      número de consumidores
 *   --buffer S    tamanho do buffer
 *   --runtime T   tempo total de execução (segundos)
 *
 * No final, imprime as metricas
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include "buffer.h"
#include "metrics.h"

#define DEFAULT_BUFFER_SIZE 7
#define DEFAULT_PROD        2
#define DEFAULT_CONS        4
#define DEFAULT_RUNTIME    10

Buffer buffer;
pthread_mutex_t mutex;
sem_t empty, full; // Semáforos: empty conta vagas livres, full conta itens disponíveis
Metrics metrics;

int produced_count = 0; // Contador total de itens produzidos
int consumed_count = 0; // Contador total de itens consumidos

// Variáveis configuráveis por CLI
int numProd = DEFAULT_PROD;
int numCons = DEFAULT_CONS;
int buffer_size = DEFAULT_BUFFER_SIZE;
int runtime_seconds = DEFAULT_RUNTIME;

//thread produtora
void *producer(void *param) {
    int item;
    while (1) {
        sleep(1);
        item = rand() % 100;

        sem_wait(&empty); // Decrementa empty (espera vaga livre)
        pthread_mutex_lock(&mutex); // Entra na seção crítica garantindo exclusão mútua

        if (insert_item(&buffer, item) == 0) {
            produced_count++; //incrementa métrica de itens produzidos

            //geracao da metricas
            record_production(&metrics, item);
            printf("[SEM %lu] Produzido: %d | count=%d\n",
                   (unsigned long)pthread_self(), item, buffer.count);
        } else {
            printf("[SEM %lu] Erro: buffer cheio (item %d descartado)\n",
                   (unsigned long)pthread_self(), item);
        }

        pthread_mutex_unlock(&mutex); // Sai da seção crítica
        sem_post(&full); // Incrementa full (há pelo menos um item disponível)
    }
    return NULL;
}

//thread consumidora
void *consumer(void *param) {
    int item;
    while (1) {
        sleep(2);

        sem_wait(&full);// Decrementa empty (espera vaga livre)
        pthread_mutex_lock(&mutex); //exclusão mútua - seção crítica 

        if (remove_item(&buffer, &item) == 0) {
            consumed_count++;// Atualiza métrica de itens consumidos

            //geracao das metricas
            for (int i = 0; i < metrics.total_produced; i++) {
                if (metrics.produced_items[i].item == item) {
                    record_consumption(&metrics, item, metrics.produced_items[i].production_time);
                    break;
                }
            }
            printf("[SEM %lu] Consumido: %d | count=%d\n",
                   (unsigned long)pthread_self(), item, buffer.count);
        } else {
            printf("[SEM %lu] Erro: buffer vazio\n",
                   (unsigned long)pthread_self());
        }

        pthread_mutex_unlock(&mutex); // fim da seção crítica
        sem_post(&empty);             // Incrementa empty (há pelo menos uma vaga livre)
    }
    return NULL;
}
// Thread para amostrar a utilização do buffer
void *monitor_buffer(void *param) {
    while (1) {
        usleep(100000); // amostra a cada 100ms
        sample_buffer_usage(&metrics, buffer.count, buffer.capacity);
    }
    return NULL;   
}

void print_usage(char *prog_name) {
    printf("Uso: %s [--prod N] [--cons M] [--buffer S] [--runtime T]\n", prog_name);
}

int main(int argc, char *argv[]) {
    srand((unsigned)time(NULL));

    // struct das opções long para usar no getopt_long
    static struct option long_options[] = {
        {"prod",    required_argument, 0, 'p'},
        {"cons",    required_argument, 0, 'c'},
        {"buffer",  required_argument, 0, 'b'},
        {"runtime", required_argument, 0, 'r'},
        {"help",    no_argument,       0, 'h'},
        {0,0,0,0}
    };
    int opt, idx = 0;

    // Loop de parsing das opções
    while ((opt = getopt_long(argc, argv, "p:c:b:r:h", long_options, &idx)) != -1) {
        switch (opt) {
            case 'p': numProd        = atoi(optarg); break;
            case 'c': numCons        = atoi(optarg); break;
            case 'b': buffer_size    = atoi(optarg); break;
            case 'r': runtime_seconds= atoi(optarg); break;
            default:  print_usage(argv[0]); exit(EXIT_SUCCESS);
        }
    }

    // Inicializa buffer e sincronização
    if (init_buffer(&buffer, buffer_size) != 0) {
        fprintf(stderr, "Erro ao alocar buffer\n");
        exit(EXIT_FAILURE);
    }

    init_metrics(&metrics, buffer_size); 

    // Cria arrays dinâmicos de threads
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, buffer.capacity);
    sem_init(&full,  0, 0);

    pthread_t prod[numProd], cons[numCons], monitor_thread;
    for (int i = 0; i < numProd; i++)
        pthread_create(&prod[i], NULL, producer, NULL);
    for (int i = 0; i < numCons; i++)
        pthread_create(&cons[i], NULL, consumer, NULL);

    pthread_create(&monitor_thread, NULL, monitor_buffer, NULL);

    // Mantém o programa rodando pelo tempo definido
    sleep(runtime_seconds);

    printf("\n== EXECUCAO FINALIZADA (SEM) ==\n");
    printf("Itens produzidos: %d\n", produced_count);
    printf("Itens consumidos: %d\n", consumed_count);

    calculate_metrics(&metrics, runtime_seconds);
    // mata todas as threads
    exit(0);
}
