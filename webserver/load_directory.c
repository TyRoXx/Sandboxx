#include "load_directory.h"
#include "directory.h"
#include <stdio.h>


static const char *find_char_and_skip(
	const char **pos,
	const char *end,
	char c)
{
	while ((*pos != end) &&
		(**pos != c))
	{
		++(*pos);
	}

	if (*pos != end)
	{
		++(*pos);
	}
	return *pos;
}

typedef struct string_ref_t
{
	const char *begin, *end;
}
string_ref_t;


static bool is_string_ref_empty(const string_ref_t *str)
{
	return (str->begin == str->end);
}


typedef struct line_t
{
	string_ref_t name;
	string_ref_t handler;
	string_ref_t args;
}
line_t;


static void parse_line(
	line_t *line,
	const char **pos,
	const char *end)
{
	const string_ref_t name = {*pos, find_char_and_skip(pos, end, ' ')};
	const string_ref_t handler = {*pos, find_char_and_skip(pos, end, ' ')};
	const string_ref_t args = {*pos, find_char_and_skip(pos, end, '\n')};

	line->name = name;
	line->handler = handler;
	line->args = args;
}

static bool expect_not_empty(const string_ref_t *str, const char *message)
{
	if (is_string_ref_empty(str))
	{
		fputs(message, stderr);
		return false;
	}

	return true;
}

bool load_directory(
	struct directory_t *directory,
	const char *begin,
	const char *end
	)
{
	line_t line;
	const char *pos = begin;

	for (;;)
	{
		parse_line(&line, &pos, end);
		if (is_string_ref_empty(&line.name))
		{
			break;
		}

		if (!expect_not_empty(&line.handler, "Handler name expected\n") ||
			!expect_not_empty(&line.args, "Handler arguments expected\n"))
		{
			return false;
		}

		//TODO
	}

	return true;
}
