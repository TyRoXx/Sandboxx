#include "http_request.h"
#include "common/buffer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


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

		if (c == '\r')
		{
			return false;
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
	bool is_error = false;
	buffer_t method, url;

	buffer_create(&method);
	buffer_create(&url);
	WS_GEN_VECTOR_CREATE(request->headers);

	if (scan_c_string_until(' ', 64, read_byte, data, &method) &&
		scan_c_string_until(' ', 1024, read_byte, data, &url) &&
		skip_line(read_byte, data))
	{
		request->method = method.data;
		request->url = url.data;
		request->host = "";

		for (;;)
		{
			buffer_t key;
			buffer_create(&key);

			if (scan_c_string_until(':', 1024, read_byte, data, &key))
			{
				buffer_t value;

				buffer_create(&value);

				if (skip_char(read_byte, data) &&
					scan_c_string_until('\r', 4096, read_byte, data, &value))
				{
					bool success;
					char * const header_elements[2] = {key.data, value.data};

					WS_GEN_VECTOR_APPEND_RANGE(
						request->headers,
						header_elements,
						header_elements + sizeof(header_elements) / sizeof(*header_elements),
						success);

					if (success)
					{
						if (!strcmp(key.data, "Host"))
						{
							request->host = value.data;
						}

						/* skip \n */
						skip_char(read_byte, data);

						continue;
					}
				}
				else
				{
					fprintf(stderr, "Syntax error: Header value expected\n");
				}

				is_error = true;
				buffer_destroy(&value);
			}

			buffer_destroy(&key);
			break;
		}
	}
	else
	{
		is_error = true;
	}

	if (is_error)
	{
		buffer_destroy(&url);
		buffer_destroy(&method);
		WS_GEN_VECTOR_DESTROY(request->headers);
	}
	return !is_error;
}

void http_request_destroy(
	http_request_t *request)
{
	char **begin = WS_GEN_VECTOR_BEGIN(request->headers);
	char ** const end = WS_GEN_VECTOR_END(request->headers);

	for (; begin != end; ++begin)
	{
		free(*begin);
	}

	WS_GEN_VECTOR_DESTROY(request->headers);
	free(request->method);
	free(request->url);
}
