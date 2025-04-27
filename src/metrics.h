// metrics.h
#ifndef METRICS_H
#define METRICS_H

#include <time.h>
#include <sys/time.h>

typedef struct {
    struct timeval production_time;
    int item;
} ProducedItem;

typedef struct {
    int total_produced;
    int total_consumed;
    ProducedItem *produced_items;
    int produced_items_capacity;
    double total_latency;
    double total_buffer_usage;
    int usage_samples;
} Metrics;

void init_metrics(Metrics *metrics, int capacity);
void record_production(Metrics *metrics, int item);
void record_consumption(Metrics *metrics, int item, struct timeval production_time);
void calculate_metrics(Metrics *metrics, double runtime_seconds);
void sample_buffer_usage(Metrics *metrics, int buffer_count, int buffer_capacity);


#endif