#include "buffer.h"
#include <stdlib.h>
#include <string.h>


void buffer_create(buffer_t *b)
{
	b->data = 0;
	b->size = b->capacity = 0;
}

void buffer_destroy(buffer_t *b)
{
	free(b->data);
}

bool buffer_reserve(buffer_t *b, size_t capacity)
{
	char *new;
	size_t min_capacity;

	if (b->capacity >= capacity)
	{
		return true;
	}

	min_capacity = (b->capacity * 2);
	if (capacity < min_capacity)
	{
		capacity = min_capacity;
	}

	new = realloc(b->data, capacity);
	if (new)
	{
		b->capacity = capacity;
		b->data = new;
		return true;
	}

	return false;
}

bool buffer_resize(buffer_t *b, size_t size)
{
	if (!buffer_reserve(b, size))
	{
		return false;
	}

	b->size = size;
	return true;
}

bool buffer_push_back(buffer_t *b, char c)
{
	return buffer_append(b, &c, 1);
}

bool buffer_append(buffer_t *b, const void *data, size_t length)
{
	const size_t new_size = b->size + length;
	if (!buffer_reserve(b, new_size))
	{
		return false;
	}

	memcpy(b->data + b->size, data, length);
	b->size = new_size;
	return true;
}

void buffer_erase(buffer_t *b, char *begin, char *end)
{
	memmove(begin, end, (b->data + b->size) - end);
	b->size -= (end - begin);
}

void buffer_clear(buffer_t *b)
{
	b->size = 0;
}
