// src/main.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include "buffer.h"

#define DEFAULT_BUFFER_SIZE 7
#define DEFAULT_PROD        2
#define DEFAULT_CONS        4
#define DEFAULT_RUNTIME    10

Buffer buffer;
pthread_mutex_t mutex;
sem_t empty, full;

int produced_count = 0;
int consumed_count = 0;

int numProd = DEFAULT_PROD;
int numCons = DEFAULT_CONS;
int buffer_size = DEFAULT_BUFFER_SIZE;
int runtime_seconds = DEFAULT_RUNTIME;

void *producer(void *param) {
    int item;
    while (1) {
        sleep(1);
        item = rand() % 100;

        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        if (insert_item(&buffer, item) == 0) {
            produced_count++;
            printf("[SEM %lu] Produzido: %d | count=%d\n",
                   (unsigned long)pthread_self(), item, buffer.count);
        } else {
            printf("[SEM %lu] Erro: buffer cheio (item %d descartado)\n",
                   (unsigned long)pthread_self(), item);
        }

        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
    return NULL;
}

void *consumer(void *param) {
    int item;
    while (1) {
        sleep(2);

        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        if (remove_item(&buffer, &item) == 0) {
            consumed_count++;
            printf("[SEM %lu] Consumido: %d | count=%d\n",
                   (unsigned long)pthread_self(), item, buffer.count);
        } else {
            printf("[SEM %lu] Erro: buffer vazio\n",
                   (unsigned long)pthread_self());
        }

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
    return NULL;
}

void print_usage(char *prog_name) {
    printf("Uso: %s [--prod N] [--cons M] [--buffer S] [--runtime T]\n", prog_name);
}

int main(int argc, char *argv[]) {
    srand((unsigned)time(NULL));

    static struct option long_options[] = {
        {"prod",    required_argument, 0, 'p'},
        {"cons",    required_argument, 0, 'c'},
        {"buffer",  required_argument, 0, 'b'},
        {"runtime", required_argument, 0, 'r'},
        {"help",    no_argument,       0, 'h'},
        {0,0,0,0}
    };
    int opt, idx = 0;
    while ((opt = getopt_long(argc, argv, "p:c:b:r:h", long_options, &idx)) != -1) {
        switch (opt) {
            case 'p': numProd        = atoi(optarg); break;
            case 'c': numCons        = atoi(optarg); break;
            case 'b': buffer_size    = atoi(optarg); break;
            case 'r': runtime_seconds= atoi(optarg); break;
            default:  print_usage(argv[0]); exit(EXIT_SUCCESS);
        }
    }

    if (init_buffer(&buffer, buffer_size) != 0) {
        fprintf(stderr, "Erro ao alocar buffer\n");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, buffer.capacity);
    sem_init(&full,  0, 0);

    pthread_t prod[numProd], cons[numCons];
    for (int i = 0; i < numProd; i++)
        pthread_create(&prod[i], NULL, producer, NULL);
    for (int i = 0; i < numCons; i++)
        pthread_create(&cons[i], NULL, consumer, NULL);

    sleep(runtime_seconds);

    printf("\n== EXECUCAO FINALIZADA (SEM) ==\n");
    printf("Itens produzidos: %d\n", produced_count);
    printf("Itens consumidos: %d\n", consumed_count);

    // exit mata todas as threads
    exit(0);
}
