/*
The Producer-Consumer problem, versão sincronizada com o uso de mutex
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 7

pthread_mutex_t mutex;
sem_t full, empty;

int buffer [BUFFER_SIZE];
int count;

pthread_t id; // ID da Thread

void *producer(void *param);
void *consumer(void *param);

int insert_item(int item){
    if (count < BUFFER_SIZE) {
        buffer[count] = item;
        count++;
        return 0;
    }
    else{
        return -1;
    }
}

int remove_item(int *item){
    if (count > 0){
        *item = buffer[(count -1)];
        count--;
        return 0;
    }
    else{
        return -1;
    }
}

void initialize_data(){
    count = 0;

    pthread_mutex_init(&mutex, NULL);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFER_SIZE);
}

void *producer(void *param){
    int item;

    while(1){
        sleep(1);

        item = rand() %100;

        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        if(!insert_item(item)){
            printf("Produzido: %d\n", item);
        }
        else{
            printf("Buffer cheio, nao foi possivel produzir: %d\n", item);
        }

        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}

void *consumer(void *param){
    int item;
    while(1){
        sleep(3);

        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        if(!remove_item(&item)){
            printf("Consumido: %d\n", item);
        } else {
            printf("Buffer vazio, nao foi possivel consumir\n");
        }
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);        
    }
}

int main() {
    srand((unsigned int)time(NULL));

    int mainSleepTime = 20;
    int qtdProducer = 5;
    int qtrConsumer = 2;

    initialize_data();

    for(int i = 0; i < qtdProducer; i++){
        pthread_create(&id, NULL, producer, NULL);
    }

    for(int i = 0; i< qtrConsumer; i++){
        pthread_create(&id, NULL, consumer, NULL);
    }

    sleep(mainSleepTime);

    printf("Encerrando programa\n");
    exit(0);
}