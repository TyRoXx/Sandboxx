#include "http_request.h"
#include "common/buffer.h"
#include <stdlib.h>
#include <string.h>


static bool zero_terminate(buffer_t *b)
{
	return buffer_push_back(b, '\0');
}

static bool is_eof(int c)
{
	return (c < 0);
}

static bool scan_c_string_until(
	char terminator,
	size_t max_length,
	int (*read_byte)(void *),
	void *data,
	buffer_t *destination)
{
	for (;;)
	{
		int const c = read_byte(data);
		if (is_eof(c))
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
	int (*read_byte)(void *),
	void *data)
{
	int c = read_byte(data);
	for (;;)
	{
		if (is_eof(c))
		{
			return false;
		}
		else if (c == '\r')
		{
			c = read_byte(data);
			if (c == '\n')
			{
				return true;
			}
		}
		else
		{
			c = read_byte(data);
		}
	}
}

static bool skip_char(
	int (*read_byte)(void *),
	void *data)
{
	return !is_eof(
		read_byte(data)
		);
}

bool http_request_parse(
	http_request_t *request,
	int (*read_byte)(void *),
	void *data)
{
	buffer_t method, url, host_key, host;
	buffer_create(&method);
	buffer_create(&url);
	buffer_create(&host_key);
	buffer_create(&host);

	if (scan_c_string_until(' ', 4 /*POST*/, read_byte, data, &method) &&
		scan_c_string_until(' ', 1024, read_byte, data, &url) &&
		skip_line(read_byte, data) &&
		scan_c_string_until(':', 4 /*Host*/, read_byte, data, &host_key) &&
		skip_char(read_byte, data) &&
		scan_c_string_until('\r', 1024, read_byte, data, &host))
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
