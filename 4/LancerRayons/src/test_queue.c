#include "queue.h"
#include <stdlib.h>
#include <stdio.h>


int main(void)
{
	long i;
	Queue * q = queue_init(10);
	for (i = 0; i < 10; ++i)
	{
		queue_push(q, i);
	}
	for (i = 0; i < 10; ++i)
	{
		long cur = queue_pop(q);
		fprintf(stderr,"current: %ld, expected %ld\n", cur, i);
	}
	for (i = 10; i < 20; ++i)
	{
		queue_push(q, i);
	}
	for (i = 10; i < 20; ++i)
	{
		long cur = queue_pop(q);
		fprintf(stderr,"current: %ld, expected %ld\n", cur, i);
	}
	fprintf(stderr,"La pile est vide? %d (should be 1)\n", queue_isEmpty(q));
	queue_delete(q);
	return 0;
}