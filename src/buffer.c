#include "buffer.h"
#include <stdlib.h>

int init_buffer(Buffer *buffer, int capacity) {
    buffer->data = (int*) malloc(capacity * sizeof(int));
    if (!buffer->data)
        return -1;
    buffer->capacity = capacity;
    buffer->count = 0;
    buffer->front = 0;
    buffer->rear = 0;
    return 0;
}

void destroy_buffer(Buffer *buffer) {
    if (buffer->data)
        free(buffer->data);
}

int insert_item(Buffer *buffer, int item) {
    if(buffer->count == buffer->capacity)
        return -1; // Buffer cheio
    buffer->data[buffer->rear] = item;
    buffer->rear = (buffer->rear + 1) % buffer->capacity;
    buffer->count++;
    return 0;
}

int remove_item(Buffer *buffer, int *item) {
    if(buffer->count == 0)
        return -1; // Buffer vazio
    *item = buffer->data[buffer->front];
    buffer->front = (buffer->front + 1) % buffer->capacity;
    buffer->count--;
    return 0;
}
