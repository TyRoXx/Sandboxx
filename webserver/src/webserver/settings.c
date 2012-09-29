#include "settings.h"
#include "common/string_util.h"
#include <string.h>
#include <stdio.h>


void sub_domain_create(sub_domain_t *d, char *name, char *destination)
{
	d->name = name;
	d->destination = destination;
}

void sub_domain_destroy(sub_domain_t *d)
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

static bool parse_settings(settings_t *s, const char *pos, const char *end)
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
			if (!strcmp("subdomain", command))
			{
				sub_domain_t sub_domain;
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

				sub_domain_create(&sub_domain, name, location);
				WS_GEN_VECTOR_PUSH_BACK(s->sub_domains, sub_domain);
			}
			else
			{
				fprintf(stderr, "Unknown command '%s'", command);
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
	WS_GEN_VECTOR_CREATE(s->sub_domains);

	return parse_settings(s, begin, end);
}

void settings_destroy(settings_t *s)
{
	sub_domain_t * begin = WS_GEN_VECTOR_BEGIN(s->sub_domains);
	sub_domain_t * const end = WS_GEN_VECTOR_END(s->sub_domains);

	for (; begin != end; ++begin)
	{
		sub_domain_destroy(begin);
	}

	WS_GEN_VECTOR_DESTROY(s->sub_domains);
}
