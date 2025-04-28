/**
 * buffer.c
 * 
 * implementa as funções para manipulação de um buffer circular,
 * incluindo inicialização, destruição, inserção e remoção de itens.
 * O buffer é usado para armazenar elementos, permitindo
 * operações de leitura e escrita em um contexto multi-threaded (combinado
 * com mecanismos de sincronização, como semáforos ou mutexes).
 * 
 * Propósito:
 * - Gerenciar um buffer circular de tamanho fixo, que pode ser utilizado
 *   nas implementações do problema Produtor-Consumidor desse projeto.
 * - Suportar operações de inserção e remoção com comportamento FIFO (First In, First Out).
 * 
 * Funções principais:
 * - `init_buffer`: Inicializa a estrutura do buffer.
 * - `destroy_buffer`: Libera a memória alocada para o buffer.
 * - `insert_item`: Insere um item no buffer, caso não esteja cheio.
 * - `remove_item`: Remove um item do buffer, caso não esteja vazio.
 */

 #include "buffer.h"
 #include <stdlib.h>
 
 /**
  * Inicializa o buffer circular com a capacidade especificada.
  * 
  * @param buffer: Ponteiro para a estrutura do buffer inicializada.
  * @param capacity: Capacidade máxima do buffer.
  * @return 0 em caso de sucesso, -1 em caso de falha (ex.: falha na alocação de memória).
  */
 int init_buffer(Buffer *buffer, int capacity) {
     // Aloca memória para o array que armazenará os itens do buffer.
     buffer->data = (int*) malloc(capacity * sizeof(int));
     if (!buffer->data) 
         return -1;
     
     // atributos do buffer.
     buffer->capacity = capacity; // Capacidade máxima do buffer.
     buffer->count = 0;           // Número atual de itens no buffer.
     buffer->front = 0;           // Índice do início (primeiro item a ser removido).
     buffer->rear = 0;            // Índice do final (próximo local para inserção).
     return 0;
 }
 
 /**
  * Libera os recursos alocados pelo buffer.
  * 
  * @param buffer: Ponteiro para a estrutura do buffer que será destruída.
  */
 void destroy_buffer(Buffer *buffer) {
     // Libera a memória alocada para os dados do buffer, caso o buffer tenha sido inicializado.
     if (buffer->data)
         free(buffer->data);
 }
 
 /**
  * Insere um item no buffer circular.
  * 
  * @param buffer: Ponteiro para a estrutura do buffer.
  * @param item: Item inserido no buffer.
  * @return 0 em caso de sucesso, -1 se o buffer estiver cheio.
  */
 int insert_item(Buffer *buffer, int item) {
     if (buffer->count == buffer->capacity) // Verifica se o buffer está cheio.
         return -1; // Retorna erro se tiver cheio.
     
     // Insere o item na posição indicada por `rear` e atualiza o índice.
     buffer->data[buffer->rear] = item;
     buffer->rear = (buffer->rear + 1) % buffer->capacity; // Move `rear` circularmente no buffer.
     buffer->count++; // Incrementa o número de itens no buffer.
     return 0;
 }
 
 /**
  * Remove um item do buffer circular.
  * 
  * @param buffer: Ponteiro para a estrutura do buffer.
  * @param item: Ponteiro para armazenar o item removido.
  * @return 0 em caso de sucesso, -1 se o buffer estiver vazio.
  */
 int remove_item(Buffer *buffer, int *item) {
     if (buffer->count == 0) // Verifica se o buffer está vazio.
         return -1; // Retorna erro se não tem item pra remover.
     
     // Remove o item da posição indicada por `front` e atualiza o índice.
     *item = buffer->data[buffer->front];
     buffer->front = (buffer->front + 1) % buffer->capacity; // Move `front` circularmente no buffer.
     buffer->count--; // Decrementa o número de itens no buffer.
     return 0;
 }