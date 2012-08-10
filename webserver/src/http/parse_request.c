#include "parse_request.h"
#include "common/buffer.h"
#include <stdlib.h>
#include <string.h>


typedef struct parser_t
{
	istream_t *source;
	size_t pos;
	bool ended;
}
parser_t;


static int peek(parser_t *p)
{
	if (!p->ended)
	{
		const size_t old_size = istream_size(p->source);

		if (p->pos == old_size)
		{
			istream_fetch(p->source);
		}

		if (p->pos == old_size)
		{
			p->ended = true;
		}
	}

	if (p->ended)
	{
		return -1;
	}
	else
	{
		char c = p->source->begin[p->pos];
		return c;
	}
}

static bool parse_until(
	buffer_t *content,
	char delimiter,
	parser_t *parser)
{
	for (;;)
	{
		const int c = peek(parser);

		if (c == -1)
		{
			return false;
		}

		else if (c == delimiter)
		{
			return buffer_push_back(content, '\0');
		}

		else if (!buffer_push_back(content, (char) c))
		{
			return false;
		}

		++(parser->pos);
	}
}

static bool parse_delimited(
	buffer_t *content,
	const char *delimiter,
	parser_t *parser)
{
	if (parse_until(content, *delimiter, parser))
	{
		++delimiter;

		while (*delimiter != '\0')
		{
			if (*delimiter != peek(parser))
			{
				return false;
			}

			++(parser->pos);
			++delimiter;
		}

		return true;
	}

	return false;
}

static bool skip(
	const char *str,
	parser_t *p)
{
	const size_t begin_pos = p->pos;

	for (; *str != '\0'; ++str, ++(p->pos))
	{
		if (*str != peek(p))
		{
			p->pos = begin_pos;
			return false;
		}
	}

	return true;
}

bool parse_request(
	http_request_t *request,
	istream_t *source
	)
{
	bool result = false;
	parser_t parser = {source, 0, false};
	buffer_t method, url, protocol;
	buffer_create(&method);
	buffer_create(&url);
	buffer_create(&protocol);

	if (!parse_delimited(&method, " ", &parser) ||
		!parse_delimited(&url, " ", &parser) ||
		!parse_delimited(&protocol, "\r\n", &parser))
	{
		goto on_return;
	}

	while (!skip("\r\n", &parser))
	{
		buffer_t key, value;
		buffer_create(&key);
		buffer_create(&value);

		if (!parse_delimited(&key, ": ", &parser) ||
			!parse_delimited(&value, "\r\n", &parser))
		{
			buffer_destroy(&key);
			buffer_destroy(&value);
			goto on_return;
		}

		buffer_destroy(&key);
		buffer_destroy(&value);
	}

	result = true;

on_return:
	buffer_destroy(&method);
	buffer_destroy(&url);
	buffer_destroy(&protocol);
	return result;
}
