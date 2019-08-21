#include "queue.h"
#include <stdlib.h>

QUEUE queue_create() {
	QUEUE q;
	q.bottom = q.top = NULL;
	return q;
}
int queue_isEmpty(QUEUE q) {
	return q.bottom == NULL;
}

/* Adiciona um elemento à pilha */
void queue_push(QUEUE * q, int key) {
	QUEUENODE * n = (QUEUENODE *) malloc(sizeof(QUEUENODE));
	n->key = key;
	n->prev = NULL;
	if(queue_isEmpty(*q)) {
		q->top = q->bottom = n;
		return;
	}
	q->top->prev = n;
	q->top = n;
}

int queue_pop(QUEUE * q, int * key) {
	if(queue_isEmpty(*q)) return 1; //erro: pilha vazia
	if(key) (*key) = q->bottom->key;
	QUEUENODE * n = q->bottom->prev;
	free(q->bottom);
	q->bottom = n;
	return 0; // ok
}
void queue_free(QUEUE * q) {
	while(!queue_pop(q, NULL));	
}