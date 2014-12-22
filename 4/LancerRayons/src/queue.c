#include "queue.h"

#include <stdlib.h>
#include <assert.h>

Queue* queue_init(int size)
{
	Queue * q = (Queue *) malloc(sizeof(Queue));
	q->elements = (long*) malloc(size*sizeof(long));
	q->front = 0;
	q->rear = 0;
	q->cur_size = 0;
	q->size = size;
	return q;
}


int queue_isEmpty(Queue* q){
	return (q->front == q->rear);
}

void queue_push(Queue* q, long element){
	assert(q->cur_size < q->size);
	int front = q->front;
	q->elements[front] = element;
	q->front = (q->front +1) % q->size;
	q-> cur_size ++;
}
long queue_pop(Queue* q){
	assert(q->cur_size >=0);
	long element= q->elements[q->rear];
	q-> cur_size --;
	q->rear = (q->rear +1) % q->size;
	return element;
}

void queue_delete(Queue* q){
	free(q-> elements);
	free(q);
}

