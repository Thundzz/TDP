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
	for (i = 11; i < 23; ++i)
	{
		queue_push(q, i);
	}
	for (i = 11; i < 23; ++i)
	{
		long cur = queue_pop(q);
		fprintf(stderr,"current: %ld, expected %ld\n", cur, i);
	}
	queue_delete(q);
	return 0;
}