#include "stack.h"


void stack_create(stack *s, size_t element_size)
{
	vector_create(&s->storage, element_size);
}

void stack_destroy(stack *s)
{
	vector_destroy(&s->storage);
}

int stack_empty(const stack *s)
{
	return vector_empty(&s->storage);
}

int stack_push(stack *s, const void *element)
{
	return vector_push_back(&s->storage, element);
}

void stack_pop(stack *s)
{
	vector_pop_back(&s->storage);
}

void stack_clear(stack *s)
{
	vector_clear(&s->storage);
}

void *stack_top(stack *s)
{
	return vector_back(&s->storage);
}
