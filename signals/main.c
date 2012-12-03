#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef void (*slot)(void *, void *);

typedef struct signal signal;
typedef struct connection connection;

static void signal_disconnect(signal *s, connection *c);

struct connection
{
	slot callback;
	void *user_data;
	connection *previous, *next;
	size_t external_refs;
	signal *parent;
	int is_connected;
};

static void connection_remove(connection *c);
static void connection_remove_if_disconnected(connection *c);
static void connection_destroy(connection *c);
static void connection_grab(connection *c);
static void connection_drop(connection *c);
static int connection_is_connected(connection const *c);
static void connection_disconnect(connection *c);

struct signal
{
	connection *first, *last;
	size_t call_depth;
};

static void signal_create(signal *s);
static void signal_destroy(signal *s);
static connection *signal_connect(signal *s, slot callback, void *user_data);
static void signal_disconnect(signal *s, connection *c);
static void signal_call(signal *s, void *arguments);


static void connection_remove(connection *c)
{
	signal *s;
	assert(c);

	s = c->parent;
	assert(s);

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

static void connection_remove_if_disconnected(connection *c)
{
	assert(c);
	assert(!c->external_refs);

	if (!c->is_connected)
	{
		connection_remove(c);
	}
}

static void connection_destroy(connection *c)
{
	assert(c);
	free(c->user_data);
}

static void connection_grab(connection *c)
{
	assert(c);
	++(c->external_refs);
}

static void connection_drop(connection *c)
{
	assert(c);
	assert(c->external_refs > 0);

	--(c->external_refs);
	if (c->external_refs == 0)
	{
		connection_remove_if_disconnected(c);
	}
}

static int connection_is_connected(connection const *c)
{
	assert(c);
	return c->is_connected;
}

static void connection_disconnect(connection *c)
{
	assert(c);

	if (!c->is_connected)
	{
		return;
	}

	signal_disconnect(c->parent, c);
}


static void signal_create(signal *s)
{
	assert(s);
	s->first = s->last = 0;
	s->call_depth = 0;
}

static void signal_destroy(signal *s)
{
	connection *c;
	assert(s);
	assert(!s->call_depth);

	c = s->first;
	while (c)
	{
		connection * const copy = c;
		connection_destroy(c);
		c = c->next;
		free(copy);
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

static void signal_disconnect(signal *s, connection *c)
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

static void signal_call(signal *s, void *arguments)
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

static void test_connection_grab_drop(void)
{
	signal s;
	connection *c;

	signal_create(&s);

	c = signal_connect(&s, test_signal_add_callback, 0);
	assert(c);
	assert(connection_is_connected(c));

	connection_grab(c);
	connection_drop(c);

	assert(connection_is_connected(c));

	signal_destroy(&s);
}

static void test_connection_multi_grab_drop(void)
{
	enum
	{
		grab_count = 32
	};
	signal s;
	connection *c;
	int i;

	signal_create(&s);

	c = signal_connect(&s, test_signal_add_callback, 0);
	assert(c);
	assert(connection_is_connected(c));

	for (i = 0; i < grab_count; ++i)
	{
		connection_grab(c);
	}

	for (i = 0; i < grab_count; ++i)
	{
		connection_drop(c);
	}

	assert(connection_is_connected(c));

	signal_destroy(&s);
}

static void test_connection_disconnect(void)
{
	signal s;
	connection *c;

	signal_create(&s);

	c = signal_connect(&s, test_signal_add_callback, 0);
	assert(c);
	assert(connection_is_connected(c));

	connection_grab(c);
	connection_disconnect(c);

	assert(!connection_is_connected(c));

	connection_drop(c);
	signal_destroy(&s);
}

static void test_connection_disconnect_on_call_callback(void *user_data, void *arguments)
{
	connection ** const c = user_data;
	assert(c);
	assert(!arguments);

	connection_disconnect(*c);
}

static void test_connection_disconnect_on_call(void)
{
	signal s;
	connection *c;

	signal_create(&s);

	c = signal_connect(&s, test_connection_disconnect_on_call_callback, &c);
	assert(c);
	assert(connection_is_connected(c));

	signal_call(&s, 0);

	signal_destroy(&s);
}

static void test_connection_disconnect_grabbed_on_call(void)
{
	signal s;
	connection *c;

	signal_create(&s);

	c = signal_connect(&s, test_connection_disconnect_on_call_callback, &c);
	assert(c);
	assert(connection_is_connected(c));

	connection_grab(c);

	signal_call(&s, 0);
	assert(!connection_is_connected(c));

	connection_drop(c);

	signal_destroy(&s);
}

int main(void)
{
	test_signal_call();
	test_signal_disconnect();
	test_connection_grab_drop();
	test_connection_multi_grab_drop();
	test_connection_disconnect();
	test_connection_disconnect_on_call();
	test_connection_disconnect_grabbed_on_call();

	printf("Tests finished\n");
	return 0;
}

