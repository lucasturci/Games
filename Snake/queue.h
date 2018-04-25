#ifndef _QUEUE_H_
#define _QUEUE_H_

/* Tipo */
typedef struct queue_node QUEUENODE;
struct queue_node {
	QUEUENODE * prev;
	int key;
};

typedef struct {
	QUEUENODE * bottom;
	QUEUENODE * top;
} QUEUE;

/* Funcoes */
QUEUE queue_create();
int	queue_isEmpty(QUEUE q);
void queue_push(QUEUE * q, int key);
int queue_pop(QUEUE * q, int * key); //retorna 1 se a pilha estiver vazia, e 0 se ok
void queue_free(QUEUE * q);


#endif