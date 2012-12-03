#include "connection.h"
#include "signal.h"
#include <assert.h>
#include <stdlib.h>


void connection_remove(connection *c)
{
	signal *s;
	assert(c);

	s = c->parent;
	if (s)
	{
		if (c->previous)
		{
			c->previous->next = c->next;
		}
		else
		{
			s->first = c->next;
		}

		if (c->next)
		{
			c->next->previous = c->previous;
		}
		else
		{
			s->last = c->previous;
		}
	}

	connection_destroy(c);
	free(c);
}

void connection_remove_if_disconnected(connection *c)
{
	assert(c);
	assert(!c->external_refs);

	if (!c->is_connected)
	{
		connection_remove(c);
	}
}

void connection_destroy(connection *c)
{
	assert(c);
}

void connection_grab(connection *c)
{
	assert(c);
	++(c->external_refs);
}

void connection_drop(connection *c)
{
	assert(c);
	assert(c->external_refs > 0);

	--(c->external_refs);
	if (c->external_refs == 0)
	{
		connection_remove_if_disconnected(c);
	}
}

int connection_is_connected(connection const *c)
{
	assert(c);
	return c->is_connected;
}

void connection_disconnect(connection *c)
{
	assert(c);

	if (!c->is_connected)
	{
		return;
	}

	signal_disconnect(c->parent, c);
}
