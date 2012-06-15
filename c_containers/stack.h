#ifndef STACK_H
#define STACK_H


#include "vector.h"


typedef struct stack
{
	vector storage;
}
stack;


void stack_create(stack *s, size_t element_size);
void stack_destroy(stack *s);
int stack_empty(const stack *s);
int stack_push(stack *s, const void *element);
void stack_pop(stack *s);
void stack_clear(stack *s);
void *stack_top(stack *s);


#endif
