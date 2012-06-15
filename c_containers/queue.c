#include "queue.h"


void queue_create(queue *q, size_t value_size)
{
	linked_list_create(&q->list, value_size);
}

void queue_destroy(queue *q)
{
	linked_list_destroy(&q->list);
}

int queue_push(queue *q, const void *element)
{
	return linked_list_push_back(&q->list, element);
}

const void *queue_front(const queue *q)
{
	return linked_list_front(&q->list);
}

void queue_pop(queue *q)
{
	linked_list_pop_front(&q->list);
}

void queue_clear(queue *q)
{
	linked_list_clear(&q->list);
}

size_t queue_size(const queue *q)
{
	return linked_list_size(&q->list);
}

int queue_empty(const queue *q)
{
	return linked_list_empty(&q->list);
}
