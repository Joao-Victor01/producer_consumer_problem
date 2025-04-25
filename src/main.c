//src\main.c

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include "buffer.h"

#define DEFAULT_BUFFER_SIZE 7
#define DEFAULT_PROD 2
#define DEFAULT_CONS 4
#define DEFAULT_RUNTIME 10

// Variáveis globais para sincronização
Buffer buffer;
pthread_mutex_t mutex;
sem_t empty, full;

// Métricas (contadores)
int produced_count = 0;
int consumed_count = 0;

// Parâmetros definidos na linha de comando
int numProd = DEFAULT_PROD;
int numCons = DEFAULT_CONS;
int buffer_size = DEFAULT_BUFFER_SIZE;
int runtime_seconds = DEFAULT_RUNTIME;

void *producer(void *param) {
    int item;
    while (1) {
        sleep(1); // Simula o tempo de produção
        item = rand() % 100; // Produz um item aleatório

        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        
        if (insert_item(&buffer, item) == 0) {
            produced_count++;
            printf("Produzido: %d | Buffer count: %d\n", item, buffer.count);
        } else {
            printf("Erro: Buffer cheio (insercao falhou)\n");
        }
        
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
    return NULL;
}

void *consumer(void *param) {
    int item;
    while (1) {
        sleep(2); // Simula o tempo de consumo

        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        
        if (remove_item(&buffer, &item) == 0) {
            consumed_count++;
            printf("Consumido: %d | Buffer count: %d\n", item, buffer.count);
        } else {
            printf("Erro: Buffer vazio (remoção falhou)\n");
        }
        
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
    return NULL;
}

void print_usage(char *prog_name) {
    printf("Uso: %s [--prod <num>] [--cons <num>] [--buffer <size>] [--runtime <sec>]\n", prog_name);
}

int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));
    
    // Parsing dos parâmetros via linha de comando usando getopt_long
    static struct option long_options[] = {
        {"prod", required_argument, 0, 'p'},
        {"cons", required_argument, 0, 'c'},
        {"buffer", required_argument, 0, 'b'},
        {"runtime", required_argument, 0, 'r'},
        {"help", no_argument, 0, 'h'},
        {0,0,0,0}
    };

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "p:c:b:r:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'p':
                numProd = atoi(optarg);
                break;
            case 'c':
                numCons = atoi(optarg);
                break;
            case 'b':
                buffer_size = atoi(optarg);
                break;
            case 'r':
                runtime_seconds = atoi(optarg);
                break;
            case 'h':
            default:
                print_usage(argv[0]);
                exit(EXIT_SUCCESS);
        }
    }
    
    // Inicializa o buffer (buffer circular)
    if(init_buffer(&buffer, buffer_size) != 0) {
        fprintf(stderr, "Erro ao alocar buffer\n");
        exit(EXIT_FAILURE);
    }
    
    // Inicializa as primitivas de sincronização
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, buffer.capacity);
    sem_init(&full, 0, 0);
    
    // Cria os arrays para threads produtores e consumidores
    pthread_t *producers = malloc(numProd * sizeof(pthread_t));
    pthread_t *consumers = malloc(numCons * sizeof(pthread_t));
    
    // Cria as threads produtoras
    for (int i = 0; i < numProd; i++) {
        if (pthread_create(&producers[i], NULL, producer, NULL) != 0) {
            perror("Erro ao criar thread produtor");
        }
    }
    
    // Cria as threads consumidoras
    for (int i = 0; i < numCons; i++) {
        if (pthread_create(&consumers[i], NULL, consumer, NULL) != 0) {
            perror("Erro ao criar thread consumidor");
        }
    }
    
    // Executa a simulação pelo tempo definido
    sleep(runtime_seconds);
    
    // Exibe as métricas (simples contadores)
    printf("\n== Dadis da Execucao ==\n");
    printf("Itens produzidos: %d\n", produced_count);
    printf("Itens consumidos: %d\n", consumed_count);
    

    free(producers);
    free(consumers);
    destroy_buffer(&buffer);
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    
    return 0;
}
