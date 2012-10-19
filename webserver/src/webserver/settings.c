#include "settings.h"
#include "common/string_util.h"
#include <string.h>
#include <stdio.h>


typedef enum settings_version_t
{
	version_1,
	version_2,
}
settings_version_t;

void settings_host_entry_create(settings_host_entry_t *d, char *name, char *destination)
{
	d->name = name;
	d->destination = destination;
}

void settings_host_entry_destroy(settings_host_entry_t *d)
{
	free(d->name);
	free(d->destination);
}


static void skip_current_and_next_if_equals(char const **pos, char const *end, char next)
{
	++(*pos);
	if ((*pos != end) &&
		(**pos == next))
	{
		++(*pos);
	}
}

static char *parse_line(char const **pos, char const *end)
{
	char *line;
	char const * const line_begin = *pos;
	char const *line_end;

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

static bool parse_settings(
	settings_t *s,
	settings_version_t version,
	char const *pos,
	char const *end)
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
				settings_host_entry_t host;
				char * const name = parse_line(&pos, end);
				char * const location = parse_line(&pos, end);
				bool success;

				if (!name ||
					!location)
				{
					free(name);
					free(location);
					free(command);
					return false;
				}

				settings_host_entry_create(&host, name, location);
				WS_GEN_VECTOR_PUSH_BACK(s->hosts, host, success);
				if (!success)
				{
					settings_host_entry_destroy(&host);
					return false;
				}
			}
			else if ((version >= version_2) &&
				!strcmp("plugin", command))
			{
				char * const file_name = parse_line(&pos, end);
				bool success;

				if (!file_name)
				{
					return false;
				}

				WS_GEN_VECTOR_PUSH_BACK(s->plugin_file_names, file_name, success);
				if (!success)
				{
					free(file_name);
					return false;
				}
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

bool settings_create(settings_t *s, char const *begin, char const *end)
{
	char * const version = parse_line(&begin, end);
	if (!version)
	{
		return false;
	}

	WS_GEN_VECTOR_CREATE(s->hosts);
	WS_GEN_VECTOR_CREATE(s->plugin_file_names);

	if (!strcmp("version 1", version))
	{
		free(version);
		return parse_settings(s, version_1, begin, end);
	}

	if (!strcmp("version 2", version))
	{
		free(version);
		return parse_settings(s, version_2, begin, end);
	}

	fprintf(stderr, "Unsupported settings version '%s'\n", version);
	fprintf(stderr, "The first line should be:\n");
	fprintf(stderr, "version 2\n");

	free(version);
	return false;
}

void settings_destroy(settings_t *s)
{
	{
		settings_host_entry_t * begin = WS_GEN_VECTOR_BEGIN(s->hosts);
		settings_host_entry_t * const end = WS_GEN_VECTOR_END(s->hosts);

		for (; begin != end; ++begin)
		{
			settings_host_entry_destroy(begin);
		}

		WS_GEN_VECTOR_DESTROY(s->hosts);
	}

	{
		char **begin = WS_GEN_VECTOR_BEGIN(s->plugin_file_names);
		char **end = WS_GEN_VECTOR_END(s->plugin_file_names);

		for (; begin != end; ++begin)
		{
			free(*begin);
		}

		WS_GEN_VECTOR_DESTROY(s->plugin_file_names);
	}
}
