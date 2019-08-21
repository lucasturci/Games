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
int	queue_isEmpty(QUEUE s);
void queue_push(QUEUE * s, int key);
int queue_pop(QUEUE * s, int * key); //retorna 1 se a pilha estiver vazia, e 0 caso contrario 
void queue_free(QUEUE * s);


#endif