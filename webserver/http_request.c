#include "http_request.h"
#include <stdlib.h>
#include <string.h>


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
		b->data = new;
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

static bool buffer_push_back(buffer_t *b, char c)
{
	if (!buffer_resize(b, b->size + 1))
	{
		return false;
	}

	b->data[b->size - 1] = c;
	return true;
}

static bool zero_terminate(buffer_t *b)
{
	return buffer_push_back(b, '\0');
}

static bool scan_until(
	char terminator,
	size_t max_length,
	int (*read)(void *),
	void *data,
	buffer_t *destination)
{
	for (;;)
	{
		const int c = read(data);
		if (c < 0)
		{
			return false;
		}

		if (c == terminator)
		{
			break;
		}

		if (max_length > 0)
		{
			if (!buffer_push_back(destination, (char)c))
			{
				return false;
			}

			--max_length;
		}
	}

	return zero_terminate(destination);
}

static bool skip_line(
	int (*read)(void *),
	void *data)
{
	int c = read(data);
	for (;;)
	{
		if (c < 0)
		{
			return false;
		}
		else if (c == '\r')
		{
			c = read(data);
			if (c == '\n')
			{
				return true;
			}
		}
		else
		{
			c = read(data);
		}
	}
}

static bool skip_char(
	int (*read)(void *),
	void *data)
{
	return (read(data) >= 0);
}

bool http_request_parse(
	http_request_t *request,
	int (*read)(void *),
	void *data)
{
	buffer_t method, url, host_key, host;
	buffer_create(&method);
	buffer_create(&url);
	buffer_create(&host_key);
	buffer_create(&host);

	if (scan_until(' ', 4 /*POST*/, read, data, &method) &&
		scan_until(' ', 1024, read, data, &url) &&
		skip_line(read, data) &&
		scan_until(':', 4 /*Host*/, read, data, &host_key) &&
		skip_char(read, data) &&
		scan_until('\r', 1024, read, data, &host))
	{
		if (!strcmp(method.data, "GET"))
		{
			request->method = method_get;
		}
		else if (!strcmp(method.data, "POST"))
		{
			request->method = method_post;
		}
		else
		{
			goto on_error;
		}

		if (strcmp(host_key.data, "Host"))
		{
			goto on_error;
		}

		request->url = url.data;
		request->host = host.data;

		buffer_destroy(&host_key);
		buffer_destroy(&method);
		return true;
	}
on_error:
	buffer_destroy(&host);
	buffer_destroy(&host_key);
	buffer_destroy(&url);
	buffer_destroy(&method);
	return false;
}

void http_request_destroy(
	http_request_t *request)
{
	free(request->url);
	free(request->host);
}
