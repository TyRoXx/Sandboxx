#include "server.h"
#include "client.h"
#include "common/thread.h"
#include <stdio.h>


static void client_thread_proc(void *client_ptr)
{
	client_t * const client = client_ptr;

	client_serve(client);
	client_destroy(client);

	free(client);

	thread_quit();
}

static void handle_client(
	socket_t s,
	const host_entry_t *locations_begin,
	const host_entry_t *locations_end)
{
	thread_t client_thread;
	client_t * const client = malloc(sizeof(*client));

	if (!client)
	{
		socket_destroy(s);
		return;
	}

	client_create(client, s, locations_begin, locations_end);

	if (!thread_create(&client_thread, client_thread_proc, client))
	{
		client_destroy(client);
		free(client);
	}
}


bool server_create(
	server_t *s,
	settings_t const *settings,
	unsigned port
	)
{
	if (!socket_create(&s->acceptor))
	{
		fprintf(stderr, "Could not create acceptor\n");
		return false;
	}

	if (!socket_bind(s->acceptor, port))
	{
		fprintf(stderr, "Could not bind acceptor to port %u\n", (unsigned)port);
		socket_destroy(s->acceptor);
		return false;
	}

	node_plugin_manager_create(&s->plugins);
	request_handler_manager_create(&s->request_handlers);
	return true;
}

void server_run(server_t *s)
{
	socket_t client;

	while (socket_accept(s->acceptor, &client))
	{
#if 0
		handle_client(
			client,
			locations_begin,
			locations_end
			);
#endif
	}
}

void server_destroy(server_t *s)
{
	host_entry_t *begin = WS_GEN_VECTOR_BEGIN(s->hosts);
	host_entry_t * const end = WS_GEN_VECTOR_END(s->hosts);

	for (; begin != end; ++begin)
	{
		host_entry_destroy(begin);
	}

	WS_GEN_VECTOR_DESTROY(s->hosts);

	socket_destroy(s->acceptor);
	request_handler_manager_destroy(&s->request_handlers);
	node_plugin_manager_destroy(&s->plugins);
}
