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

	if (b->capacity >= capacity)
	{
		return 1;
	}

	if (capacity < (b->capacity * 2))
	{
		capacity = (b->capacity * 2);
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
	if (!buffer_resize(b, b->size + 1))
	{
		return false;
	}

	b->data[b->size - 1] = c;
	return true;
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
