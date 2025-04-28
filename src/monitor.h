// src/monitor.h

#ifndef MONITOR_H
#define MONITOR_H

#define BUFFER_SIZE 5

// Estrutura do buffer
typedef struct {
    int data[BUFFER_SIZE];
    int count;
    int in;
    int out;
} Buffer;

// Interface do monitor
void monitor_init();
void monitor_destroy();
void monitor_insert(int item);
int monitor_remove();
void monitor_print_metrics();
int monitor_get_count();
int monitor_get_capacity();
void monitor_wakeup(); // Adicionado para liberar as threads

#endif
