#include "http_request.h"
#include "common/buffer.h"
#include <stdlib.h>
#include <string.h>


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
		request->method = method.data;

		if (strcmp(host_key.data, "Host"))
		{
			goto on_error;
		}

		request->url = url.data;
		request->host = host.data;

		buffer_destroy(&host_key);
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
	free(request->method);
	free(request->url);
	free(request->host);
}
