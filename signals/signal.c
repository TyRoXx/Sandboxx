#include "signal.h"
#include "connection.h"
#include <stdlib.h>
#include <assert.h>


void signal_create(signal *s)
{
	assert(s);
	s->first = s->last = 0;
	s->call_depth = 0;
}

void signal_destroy(signal *s)
{
	connection *c;
	assert(s);
	assert(!s->call_depth);

	c = s->first;
	while (c)
	{
		connection * const copy = c;
		c = c->next;

		if (copy->external_refs)
		{
			copy->parent = 0;
		}
		else
		{
			connection_destroy(copy);
			free(copy);
		}
	}
}

connection *signal_connect(signal *s, slot callback, void *user_data)
{
	connection *result;
	assert(s);
	assert(callback);

	result = malloc(sizeof(*result));
	if (result)
	{
		result->callback = callback;
		result->user_data = user_data;
		result->previous = s->last;
		result->next = 0;
		result->external_refs = 0;
		result->parent = s;
		result->is_connected = 1;

		if (s->last)
		{
			s->last->next = result;
		}
		else
		{
			s->first = result;
		}
		s->last = result;
	}
	return result;
}

void signal_disconnect(signal *s, connection *c)
{
	assert(s);
	assert(c);

	c->is_connected = 0;

	if (!s->call_depth &&
			!c->external_refs)
	{
		connection_remove(c);
	}
}

void signal_call(signal *s, void *arguments)
{
	connection *c;
	assert(s);

	++(s->call_depth);

	for (c = s->first; c; )
	{
		connection *next;

		if (c->is_connected)
		{
			c->callback(c->user_data, arguments);
		}

		next = c->next;

		if (!c->is_connected &&
				!c->external_refs &&
				(s->call_depth == 1))
		{
			connection_remove(c);
		}

		c = next;
	}

	--(s->call_depth);
}
