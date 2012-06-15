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
int vector_insert(vector *v, size_t position, const void *element);
int vector_insert_n(vector *v, size_t position, const void *elements, size_t count);
void *vector_data(const vector *v);

typedef void *vector_iterator;

vector_iterator vector_begin(const vector *v);
vector_iterator vector_end(const vector *v);
void vector_advance(const vector *v, vector_iterator *i, ptrdiff_t n);
void vector_next(const vector *v, vector_iterator *i);
ptrdiff_t vector_distance(const vector *v, vector_iterator begin, vector_iterator end);
vector_iterator vector_at(const vector *v, size_t index);
void *vector_deref(vector_iterator iterator);


#endif
