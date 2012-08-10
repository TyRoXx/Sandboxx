#ifndef WS_BUFFER_H
#define WS_BUFFER_H


#include <stddef.h>
#include "config.h"


typedef struct buffer_t
{
	char *data;
	size_t size;
	size_t capacity;
}
buffer_t;


void buffer_create(buffer_t *b);
void buffer_destroy(buffer_t *b);
bool buffer_reserve(buffer_t *b, size_t capacity);
bool buffer_resize(buffer_t *b, size_t size);
bool buffer_push_back(buffer_t *b, char c);
bool buffer_append(buffer_t *b, const void *data, size_t length);
void buffer_erase(buffer_t *b, char *begin, char *end);
void buffer_clear(buffer_t *b);


#endif
