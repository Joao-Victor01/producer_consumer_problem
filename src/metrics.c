// metrics.c
#include "metrics.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void init_metrics(Metrics *metrics, int capacity) {
    metrics->total_produced = 0;
    metrics->total_consumed = 0;
    metrics->produced_items = malloc(capacity * sizeof(ProducedItem));
    metrics->produced_items_capacity = capacity;
    metrics->total_latency = 0.0;
    metrics->total_buffer_usage = 0.0;
    metrics->usage_samples = 0;
}

void record_production(Metrics *metrics, int item) {
    if (metrics->total_produced < metrics->produced_items_capacity) {
        gettimeofday(&metrics->produced_items[metrics->total_produced].production_time, NULL);
        metrics->produced_items[metrics->total_produced].item = item;
        metrics->total_produced++;
    }
}

void record_consumption(Metrics *metrics, int item, struct timeval production_time) {
    struct timeval consumption_time;
    gettimeofday(&consumption_time, NULL);
    
    double latency = (consumption_time.tv_sec - production_time.tv_sec) * 1000.0;      // segundos para ms
    latency += (consumption_time.tv_usec - production_time.tv_usec) / 1000.0;   // microssegundos para ms
    
    metrics->total_latency += latency;
    metrics->total_consumed++;
}

void calculate_metrics(Metrics *metrics, double runtime_seconds) {
    double throughput_prod = metrics->total_produced / runtime_seconds;
    double throughput_cons = metrics->total_consumed / runtime_seconds;
    double avg_latency = (metrics->total_consumed > 0) ? (metrics->total_latency / metrics->total_consumed) : 0.0;
    double avg_buffer_usage = (metrics->usage_samples > 0) ? (metrics->total_buffer_usage / metrics->usage_samples * 100) : 0.0;

    printf("\n=== Metricas ===\n");
    printf("Throughput (producao): %.2f\n", throughput_prod);
    printf("Throughput (consumo):  %.2f\n", throughput_cons);
    printf("Latencia media:        %.2f\n", avg_latency);
    printf("Utilizacao do buffer:  %.2f\n", avg_buffer_usage);
}

void sample_buffer_usage(Metrics *metrics, int buffer_count, int buffer_capacity) {
    metrics->total_buffer_usage += (double) buffer_count / buffer_capacity;
    metrics->usage_samples++;
}