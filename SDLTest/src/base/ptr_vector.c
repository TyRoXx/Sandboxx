#include "ptr_vector.h"
#include <assert.h>


void PtrVector_init(PtrVector *v)
{
	Vector_init(&v->content);
}

void PtrVector_free(PtrVector *v)
{
	Vector_free(&v->content);
}

Bool PtrVector_resize(PtrVector *v, size_t size)
{
	return Vector_resize(&v->content, size * sizeof(void *));
}

Bool PtrVector_push_back(PtrVector *v, void *element)
{
	return Vector_push_back(&v->content, &element, sizeof(element));
}

size_t PtrVector_size(PtrVector const *v)
{
	return Vector_size(&v->content) / sizeof(void *);
}

void *PtrVector_get(PtrVector const *v, size_t index)
{
	assert(index < PtrVector_size(v));
	return PtrVector_begin(v)[index];
}

void PtrVector_set(PtrVector *v, size_t index, void *element)
{
	assert(index < PtrVector_size(v));
	PtrVector_begin(v)[index] = element;
}

void **PtrVector_begin(PtrVector const *v)
{
	return (void **)Vector_data(&v->content);
}

void **PtrVector_end(PtrVector const *v)
{
	return PtrVector_begin(v) + PtrVector_size(v);
}
