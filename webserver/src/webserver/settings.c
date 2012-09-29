#include "settings.h"
#include "common/string_util.h"
#include <string.h>
#include <stdio.h>


void host_entry_create(host_entry_t *d, char *name, char *destination)
{
	d->name = name;
	d->destination = destination;
}

void host_entry_destroy(host_entry_t *d)
{
	free(d->name);
	free(d->destination);
}


static void skip_current_and_next_if_equals(const char **pos, const char *end, char next)
{
	++(*pos);
	if ((*pos != end) &&
		(**pos == next))
	{
		++(*pos);
	}
}

static char *parse_line(const char **pos, const char *end)
{
	char *line;
	const char * const line_begin = *pos;
	const char *line_end;

	for (;;)
	{
		if (*pos == end)
		{
			line_end = end;
			break;
		}

		/* skip "\n" and "\n\r" */
		if (**pos == '\n')
		{
			line_end = *pos;
			skip_current_and_next_if_equals(pos, end, '\r');
			break;
		}

		/* skip "\r" and "\r\n" */
		if (**pos == '\r')
		{
			line_end = *pos;
			skip_current_and_next_if_equals(pos, end, '\n');
			break;
		}

		++(*pos);
	}

	line = malloc(line_end - line_begin + 1);
	if (line)
	{
		memcpy(line, line_begin, (line_end - line_begin));
		line[line_end - line_begin] = '\0';
	}
	return line;
}

static bool parse_settings_v1(settings_t *s, const char *pos, const char *end)
{
	while (pos != end)
	{
		char * const command = parse_line(&pos, end);
		if (!command)
		{
			return false;
		}

		if (*command != '\0')
		{
			if (!strcmp("host", command))
			{
				host_entry_t host;
				char * const name = parse_line(&pos, end);
				char * const location = parse_line(&pos, end);

				if (!name ||
					!location)
				{
					free(name);
					free(location);
					free(command);
					return false;
				}

				host_entry_create(&host, name, location);
				WS_GEN_VECTOR_PUSH_BACK(s->hosts, host);
			}
			else
			{
				fprintf(stderr, "Unknown command '%s'\n", command);
				free(command);
				return false;
			}
		}

		free(command);
	}

	return true;
}

bool settings_create(settings_t *s, const char *begin, const char *end)
{
	char * const version = parse_line(&begin, end);
	if (!version)
	{
		return false;
	}

	WS_GEN_VECTOR_CREATE(s->hosts);

	if (!strcmp("version 1", version))
	{
		free(version);
		return parse_settings_v1(s, begin, end);
	}

	fprintf(stderr, "Unsupported settings version '%s'\n", version);
	fprintf(stderr, "The first line should be:\n");
	fprintf(stderr, "version 1\n");

	free(version);
	return false;
}

void settings_destroy(settings_t *s)
{
	host_entry_t * begin = WS_GEN_VECTOR_BEGIN(s->hosts);
	host_entry_t * const end = WS_GEN_VECTOR_END(s->hosts);

	for (; begin != end; ++begin)
	{
		host_entry_destroy(begin);
	}

	WS_GEN_VECTOR_DESTROY(s->hosts);
}
