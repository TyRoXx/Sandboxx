#include "vector.h"
#include <stdlib.h>
#include <string.h>


void Vector_init(Vector *v)
{
	v->data = 0;
	v->capacity = v->size = 0;
}

void Vector_free(Vector *v)
{
	free(v->data);
}

char *Vector_release(Vector *v)
{
	return v->data;
}

Bool Vector_push_back(Vector *v, void const *element, size_t size)
{
	if (!Vector_reserve(v, v->capacity + size))
	{
		return 0;
	}
	memcpy(v->data + v->size, element, size);
	v->size += size;
	return 1;
}

size_t Vector_size(Vector const *v)
{
	return v->size;
}

Bool Vector_empty(Vector const *v)
{
	return v->size == 0;
}

char *Vector_data(Vector const *v)
{
	return v->data;
}

Bool Vector_reserve(Vector *v, size_t capacity)
{
	char *new_data;

	if (v->capacity >= capacity)
	{
		return 1;
	}

	capacity *= 2;

	new_data = realloc(v->data, capacity);
	if (!new_data)
	{
		return 0;
	}

	v->data = new_data;
	v->capacity = capacity;
	return 1;
}

Bool Vector_resize(Vector *v, size_t size)
{
	size_t const old_size = v->size;
	if (Vector_reserve(v, size))
	{
		if (size > old_size)
		{
			memset(v->data + old_size, 0, size - old_size);
		}
		v->size = size;
		return 1;
	}
	return 0;
}

char *Vector_begin(Vector const *v)
{
	return Vector_data(v);
}

char *Vector_end(Vector const *v)
{
	return Vector_data(v) + Vector_size(v);
}

Bool Vector_append_binary_file(Vector *v, FILE *in)
{
	size_t const original_size = Vector_size(v);
	size_t buffer_size = 8192;
	for (;;)
	{
		size_t const offset_to_read_to = Vector_size(v);
		size_t const max_size_t = ~(size_t)0;
		size_t resizing = original_size + buffer_size;
		size_t read_size;
		size_t actually_read;
		if (resizing > max_size_t)
		{
			resizing = max_size_t;
		}
		if (!Vector_resize(v, resizing))
		{
			/* shrinking cannot fail */
			Vector_resize(v, original_size);
			return False;
		}
		read_size = resizing - offset_to_read_to;
		actually_read = fread(Vector_data(v) + offset_to_read_to, 1, read_size, in);
		if (actually_read < read_size)
		{
			/* shrinking cannot fail */
			Vector_resize(v, offset_to_read_to + actually_read);
			return True;
		}
	}
}
