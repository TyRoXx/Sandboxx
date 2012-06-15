#ifndef QUEUE_H
#define QUEUE_H


#include <stddef.h>


typedef struct queue queue;


void queue_create(queue *q, size_t element_size);
void queue_destroy(queue *q);
void queue_push(queue *q, const void *element);
const void *queue_front(const queue *q);
const void *queue_back(const queue *q);
void queue_pop(queue *q);
void queue_clear(queue *q);


#endif
