#include "load_directory.h"
#include "directory.h"
#include "buffer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>


static const char *find_char_and_skip(
	const char **pos,
	const char *end,
	char c)
{
	const char *result;

	while ((*pos != end) &&
		(**pos != c))
	{
		++(*pos);
	}

	result = *pos;

	if (*pos != end)
	{
		++(*pos);
	}

	return result;
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

static bool equals_string_ref(const string_ref_t *left, const char *right)
{
	const size_t len = strlen(right);
	return
		((left->end - left->begin) == len) &&
		!memcmp(left->begin, right, len);
}

static char *ref_to_string(const string_ref_t *ref)
{
	const size_t len = (ref->end - ref->begin);
	char *result = malloc(len + 1);
	if (result)
	{
		memcpy(result, ref->begin, len);
		result[len] = '\0';
	}
	return result;
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

static const loadable_handler_t *find_handler(
	const loadable_handler_t *handlers_begin,
	const loadable_handler_t *handlers_end,
	const string_ref_t *name)
{
	const loadable_handler_t *h;

	for (h = handlers_begin; h != handlers_end; ++h)
	{
		if (equals_string_ref(name, h->name))
		{
			break;
		}
	}

	return h;
}

bool load_directory(
	struct directory_t *directory,
	const char *begin,
	const char *end,
	const loadable_handler_t *handlers_begin,
	const loadable_handler_t *handlers_end
	)
{
	line_t line;
	const char *pos = begin;
	buffer_t entries;

	assert(!directory->default_);
	assert(!directory->entries);
	assert(!directory->entry_count);

	buffer_create(&entries);

	for (;;)
	{
		const loadable_handler_t *handler;
		directory_entry_t entry;
		char *args;
		bool success;

		parse_line(&line, &pos, end);
		if (is_string_ref_empty(&line.name))
		{
			break;
		}

		if (!expect_not_empty(&line.handler, "Handler name expected\n") ||
			!expect_not_empty(&line.args, "Handler arguments expected\n"))
		{
			goto on_error;
		}


		handler = find_handler(handlers_begin, handlers_end, &line.handler);
		if (handler == handlers_end)
		{
			fprintf(stderr, "Unknown handler name\n");
			goto on_error;
		}
		
		args = ref_to_string(&line.args);
		if (!args)
		{
			goto on_error;
		}

		success = handler->initialize(&entry, args);

		free(args);

		if (success)
		{
			entry.name = ref_to_string(&line.name);
			if (!entry.name)
			{
				goto on_error;
			}

			if (equals_string_ref(&line.name, "*"))
			{
				if (directory->default_)
				{
					fprintf(stderr, "There can only be one default handler per directory\n");
					free(entry.name);
					goto on_error;
				}

				directory->default_ = malloc(sizeof(entry));
				if (!directory->default_)
				{
					free(entry.name);
					goto on_error;
				}

				*directory->default_ = entry;
			}
			else
			{
				if (!buffer_append(&entries, &entry, sizeof(entry)))
				{
					free(entry.name);
					goto on_error;
				}
			}
		}
		else
		{
			goto on_error;
		}
	}

	directory->entries = (directory_entry_t *)entries.data;
	directory->entry_count = (entries.size / sizeof(*directory->entries));
	return true;

on_error:
	{
		size_t i;
		for (i = 0; i < entries.size; i += sizeof(directory_entry_t))
		{
			directory_entry_t * const entry = (directory_entry_t *)(entries.data + i);
			directory_entry_destroy(entry);
		}
	}
	buffer_destroy(&entries);
	return false;
}
