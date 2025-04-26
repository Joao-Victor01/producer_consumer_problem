//src\monitor.h

#ifndef MONITOR_H
#define MONITOR_H
#define BUFFER_SIZE 5
void monitor_print_metrics();


// Estrutura do buffer
typedef struct {
    int data[BUFFER_SIZE];
    int count;
    int in;
    int out;
} Buffer;

// Interface do monitor simulado
void monitor_init();
void monitor_destroy();
void monitor_insert(int item);
int monitor_remove();

#endif
