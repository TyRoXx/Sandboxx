#ifndef VECTOR_H
#define VECTOR_H


#include <stddef.h>


typedef struct vector
{
	void *elements;
	size_t element_size;
	size_t size;
	size_t capacity;
}
vector;


void vector_create(vector *v, size_t element_size);
void vector_destroy(vector *v);
int vector_empty(const vector *v);
size_t vector_size(const vector *v);
size_t vector_capacity(const vector *v);
int vector_push_back(vector *v, const void *element);
int vector_push_front(vector *v, const void *element);
void vector_pop_back(vector *v);
void vector_pop_front(vector *v);
void *vector_get(vector *v, size_t index);
int vector_resize(vector *v, size_t size, const void *fill);
int vector_reserve(vector *v, size_t capacity);
void vector_fill(vector *v, const void *fill);
void vector_erase(vector *v, size_t position);
void vector_erase_n(vector *v, size_t begin, size_t end);
void vector_insert(vector *v, size_t position, const void *element);
void vector_insert_n(vector *v, size_t position, const void *elements, size_t count);
size_t vector_find_first(const vector *v, const void *element);
size_t vector_find_last(const vector *v, const void *element);


#endif
