#ifndef	_QUEUE_H_
#define _QUEUE_H_

/* A basic API for int queues in C */

typedef struct {
	long * elements;
	int front;
	int rear;
	int size;
	int cur_size;
} Queue;

Queue* queue_init(int size);

int queue_isEmpty(Queue* q);
int queue_length(Queue* q);

void queue_push(Queue* q, long element);
long queue_pop(Queue* q);

void queue_delete(Queue* q);


#endif