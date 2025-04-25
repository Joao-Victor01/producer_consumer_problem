//src\buffer.h

#ifndef BUFFER_H
#define BUFFER_H

typedef struct {
    int *data;       // Array que armazena os itens
    int capacity;    // Tamanho máximo do buffer
    int count;       // Número atual de itens no buffer
    int front;       // Índice de remoção (próximo item a ser consumido)
    int rear;        // Índice de inserção (próximo local livre)
} Buffer;

// Inicializa o buffer com a capacidade especificada.
// Retorna 0 em caso de sucesso, ou -1 em caso de erro.
int init_buffer(Buffer *buffer, int capacity);

// Libera a memória alocada pelo buffer.
void destroy_buffer(Buffer *buffer);

// Insere um item no buffer circular.
// Retorna 0 se a inserção foi bem-sucedida, -1 se o buffer estiver cheio.
int insert_item(Buffer *buffer, int item);

// Remove um item do buffer circular (seguindo a ordem FIFO).
// Armazena o item removido no parâmetro "item" e retorna 0 em sucesso,
// ou -1 se o buffer estiver vazio.
int remove_item(Buffer *buffer, int *item);

#endif
