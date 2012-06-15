#include "queue.h"


void queue_create(queue *q, size_t value_size)
{
	linked_list_create(q, value_size);
}

void queue_destroy(queue *q)
{
	linked_list_destroy(q);
}

void queue_push(queue *q, const void *element)
{
	linked_list_push_back(q, element);
}

const void *queue_front(const queue *q)
{
	return linked_list_front(q);
}

void queue_pop(queue *q)
{
	linked_list_pop_front(q);
}

void queue_clear(queue *q)
{
	linked_list_clear(q);
}
