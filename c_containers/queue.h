#ifndef QUEUE_H
#define QUEUE_H


#include "linked_list.h"


typedef linked_list queue;


void queue_create(queue *q, size_t value_size);
void queue_destroy(queue *q);
int queue_push(queue *q, const void *element);
const void *queue_front(const queue *q);
void queue_pop(queue *q);
void queue_clear(queue *q);
size_t queue_size(const queue *q);
int queue_empty(const queue *q);


#endif
