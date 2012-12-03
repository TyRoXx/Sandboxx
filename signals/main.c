#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef void (*slot)(void *, void *);

typedef struct connection
{
	slot callback;
	void *user_data;
	struct connection *previous, *next;
}
connection;

static void connection_destroy(connection *c)
{
	free(c->user_data);
}

typedef struct signal
{
	connection *first, *last;
}
signal;

static void signal_create(signal *s)
{
	assert(s);
	s->first = s->last = 0;
}

static void signal_destroy(signal *s)
{
	connection *c;
	assert(s);

	c = s->first;
	while (c)
	{
		connection_destroy(c);
		c = c->next;
	}
}

static connection *signal_connect(signal *s, slot callback, void *user_data)
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

static void signal_disconnect(signal *s, connection *c)
{
	assert(s);
	assert(c);

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

	connection_destroy(c);
	free(c);
}

static void signal_call(signal const *s, void *arguments)
{
	connection *c;
	assert(s);

	for (c = s->first; c; c = c->next)
	{
		c->callback(c->user_data, arguments);
	}
}

static void test_signal_add_callback(void *user_data, void *arguments)
{
	int * const counter = user_data;
	int * const step = arguments;
	*counter += *step;
}

static void test_signal_call(void)
{
	enum
	{
		connection_count = 5
	};
	signal s;
	int i;
	int found = 0;
	int step = 2;
	int const expected = (connection_count * step);

	signal_create(&s);

	for (i = 0; i < connection_count; ++i)
	{
		connection * const c = signal_connect(&s, test_signal_add_callback, &found);
		assert(c);
	}

	signal_call(&s, &step);
	assert(found == expected);

	signal_destroy(&s);
}

static void test_signal_disconnect(void)
{
	enum
	{
		connection_count = 5
	};
	static size_t const disconnect_order[connection_count] = {1, 0, 4, 2, 3};
	connection *connections[connection_count];
	signal s;
	size_t i;
	int found = 0;
	int step = 3;
	int const expected = (connection_count * step);

	signal_create(&s);

	for (i = 0; i < (size_t)connection_count; ++i)
	{
		connections[i] = signal_connect(&s, test_signal_add_callback, &found);
		assert(connections[i]);
	}

	signal_call(&s, &step);
	assert(found == expected);

	for (i = 0; i < (size_t)connection_count; ++i)
	{
		signal_disconnect(&s, connections[disconnect_order[i]]);
	}

	found = 0;
	signal_call(&s, &step);
	assert(found == 0);

	signal_destroy(&s);
}

int main(void)
{
	test_signal_call();
	test_signal_disconnect();
	printf("Tests finished\n");
	return 0;
}

