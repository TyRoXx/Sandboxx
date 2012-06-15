#include "vector.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>


void vector_create(vector *v, size_t element_size)
{
	v->elements = 0;
	v->element_size = element_size;
	v->size = 0;
	v->capacity = 0;
}

void vector_destroy(vector *v)
{
	free(v->elements);
}

int vector_empty(const vector *v)
{
	return v->size == 0;
}

size_t vector_size(const vector *v)
{
	return v->size;
}

size_t vector_capacity(const vector *v)
{
	return v->capacity;
}

int vector_push_back(vector *v, const void *element)
{
	return vector_insert(v, v->size, element);
}

int vector_push_front(vector *v, const void *element)
{
	return vector_insert(v, 0, element);
}

void vector_pop_back(vector *v)
{
	vector_erase(v, v->size - 1);
}

void vector_pop_front(vector *v)
{
	vector_erase(v, 0);
}

void *vector_get(vector *v, size_t index)
{
	return ((char *)v->elements) + (index * v->element_size);
}

int vector_resize(vector *v, size_t size, const void *fill)
{
	char *p, *end;

	if (!vector_reserve(v, size))
	{
		return 0;
	}

	p = v->elements;
	end = p + size;
	p += v->size;

	for (; p < end; p += v->element_size)
	{
		memmove(p, fill, v->element_size);
	}

	v->size = size;
	return 1;
}

int vector_reserve(vector *v, size_t capacity)
{
	size_t real_capacity;
	void *reallocated;

	if (capacity <= v->capacity)
	{
		return 1;
	}

	real_capacity = v->capacity * 2;
	if (real_capacity < capacity)
	{
		real_capacity = capacity;
	}
	if (real_capacity < 4)
	{
		real_capacity = 4;
	}

	reallocated = realloc(v->elements, real_capacity * v->element_size);
	if (!reallocated)
	{
		return 0;
	}

	v->elements = reallocated;
	v->capacity = real_capacity;
	return 1;
}

void vector_fill(vector *v, const void *fill)
{
	char *p = v->elements;
	char *end = p + (v->element_size * v->size);
	for (; p != end; p += v->element_size)
	{
		memmove(p, fill, v->element_size);
	}
}

void vector_erase(vector *v, size_t position)
{
	vector_erase_n(v, position, position + 1);
}

void vector_erase_n(vector *v, size_t begin, size_t end)
{
	const size_t erased = (end - begin);
	char *dest = ((char *)v->elements) + (begin * v->element_size);
	const char *src = ((char *)v->elements) + (end * v->element_size);

	assert(begin <= v->size);
	assert(end <= v->size);
	assert(begin <= end);
	assert(erased <= v->size);

	memmove(
		dest,
		src,
		(v->size - end)
		);

	v->size -= erased;
}

int vector_insert(vector *v, size_t position, const void *element)
{
	return vector_insert_n(v, position, element, 1);
}

int vector_insert_n(vector *v, size_t position, const void *elements, size_t count)
{
	const size_t new_size = (v->size + count);
	char *position_ptr;
	if (!vector_reserve(v, new_size))
	{
		return 0;
	}

	position_ptr = ((char *)v->elements) + (position * v->element_size);
	memmove(
		position_ptr + (count * v->element_size),
		position_ptr,
		(v->size - position) * v->element_size
		);

	memmove(
		position_ptr,
		elements,
		count * v->element_size
		);

	v->size += count;
	return 1;
}

void *vector_data(const vector *v)
{
	return v->elements;
}


vector_iterator vector_begin(const vector *v)
{
	return v->elements;
}

vector_iterator vector_end(const vector *v)
{
	return ((char *)v->elements) + (v->size * v->element_size);
}

void vector_advance(const vector *v, vector_iterator *i, ptrdiff_t n)
{
	char *p = *i;
	p += ((ptrdiff_t)v->element_size) * n;
	*i = p;
}

void vector_next(const vector *v, vector_iterator *i)
{
	vector_advance(v, i, 1);
}

ptrdiff_t vector_distance(const vector *v, vector_iterator begin, vector_iterator end)
{
	char *b = begin, *e = end;
	return (e - b) / v->element_size;
}

vector_iterator vector_at(const vector *v, size_t index)
{
	vector_iterator result = vector_begin(v);
	vector_advance(v, &result, index);
	return result;
}

void *vector_deref(vector_iterator iterator)
{
	return iterator;
}
