#include "connection.h"
#include "signal.h"
#include <stdio.h>
#include <assert.h>


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

static void test_connection_drop_after_destroy(void)
{
	signal s;
	connection *c;

	signal_create(&s);

	c = signal_connect(&s, test_connection_disconnect_on_call_callback, &c);
	assert(c);

	connection_grab(c);
	signal_destroy(&s);
	connection_drop(c);
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
	test_connection_drop_after_destroy();

	printf("Tests finished\n");
	return 0;
}
