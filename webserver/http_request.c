#include "http_request.h"
#include <stdlib.h>


static void destroy_header(http_header_t *header)
{
	free(header->key);
	free(header->value);
}

typedef struct buffer_t
{
	char *data;
	size_t size;
	size_t capacity;
}
buffer_t;

static void buffer_create(buffer_t *b)
{
	b->data = 0;
	b->size = b->capacity = 0;
}

static void buffer_destroy(buffer_t *b)
{
	free(b->data);
}

static bool buffer_reserve(buffer_t *b, size_t capacity)
{
	char *new;

	if (capacity <= b->capacity)
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
		return true;
	}

	return false;
}

static bool buffer_resize(buffer_t *b, size_t size)
{
	if (!buffer_reserve(b, size))
	{
		return false;
	}

	b->size = size;
	return true;
}

bool http_request_parse(
	http_request_t *request,
	int (*read)(void *),
	void *data)
{
	return false;
}

void http_request_destroy(
	http_request_t *request)
{
	size_t i;

	for (i = 0; i < request->header_count; ++i)
	{
		destroy_header(request->headers + i);
	}

	free(request->path);
	free(request->search);
}
