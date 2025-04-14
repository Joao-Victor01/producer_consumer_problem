/*
The Producer-Consumer problem:
O problema se origina da sincronização de processos/trheads com o uso de um buffer compartilhado:
    1. Os produtores devem parar de produzir quando o buffer estiver cheio;
    2. Os produtores devem ser sincronizados, entre si, para que não sobrescrevam os dados; (exclusão mútua)
    3. Os consumidores devem parar de consumir quando o buffer estiver vazio;
    4. Os consumidores devem ser sincronizados entre si para que não consumam o mesmo dado. (exclusão mútua)
*/

//VERSÃO SEM SICRONIZAÇÃO (sem mutex, sem semáforos, sem variáveis de condição)

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 7

int buffer[BUFFER_SIZE]; // Buffer compartilhado
int count; // Contador de itens no buffer

pthread_t tid;

void *producer(void *param);
void *consumer(void *param);

int insert_item(int item) {
    if (count < BUFFER_SIZE) { // Verifica se o buffer não está cheio
        buffer[count] = item;
        count++;
        return 0; // Sucesso
    } else {
        return -1; // Buffer cheio
    }
}

int remove_item(int *item){
    if (count > 0) { // Verifica se o buffer não está vazio
        *item = buffer[count - 1]; // Remove o item do final do buffer
        count--;
        return 0; // Sucesso
    } else {
        return -1; // Buffer vazio
    }
}

void initialize_data() {
    count = 0; // Inicializa o contador de itens no buffer
}

void* producer (void *param) {
    int item;
    while (1) {
        sleep(1); // Simula o tempo de produção
        item = rand() % 100; // Produz um item aleatório

        if(!insert_item(item)) { // Tenta inserir o item no buffer
            printf("Produzido: %d\n", item);
        } else {
            printf("Buffer cheio, nao foi possivel produzir: %d\n", item);
        }
    }
}

void* consumer (void *param) {
    int item;
    while (1) {
        sleep(2); // Simula o tempo de consumo

        if(!remove_item(&item)) { // Tenta remover um item do buffer
            printf("Consumido: %d\n", item);
        } else {
            printf("Buffer vazio, nao foi possivel consumir\n");
        }
    }
}

int main(){
    srand((unsigned int)time(NULL)); // Inicializa o gerador de números aleatórios

    int mainSleepTime = 10; // Tempo total de execução do programa
    int qtdProdutores = 2; // Quantidade de threads produtoras
    int qtdConsumidores = 4; // Quantidade de threads consumidoras

    initialize_data(); // Inicializa o buffer

    for(int i = 0; i < qtdProdutores; i++) {
        pthread_create(&tid, NULL, producer, NULL); // Cria threads produtoras
    }
    for(int i = 0; i < qtdConsumidores; i++) {
        pthread_create(&tid, NULL, consumer, NULL); // Cria threads consumidoras
    }

    sleep(mainSleepTime); // Tempo total de execução do programa
    pthread_exit(NULL); // Finaliza todas as threads
    exit(0); // Encerra o programa
}
