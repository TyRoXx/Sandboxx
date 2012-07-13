#include "socket.h"
#include "thread.h"
#include <stdio.h>
#include <stdlib.h>


static void serve_client(socket_t client)
{
}

static void client_thread_proc(void *client_ptr)
{
	socket_t client = *(socket_t *)client_ptr;
	free(client_ptr);
	serve_client(client);
	thread_quit();
}

static void handle_client(socket_t client)
{
	socket_t *client_ptr = malloc(sizeof(client));
	thread_t client_thread;

	if (!client_ptr)
	{
		return;
	}
	*client_ptr = client;
	if (!thread_create(&client_thread, client_thread_proc, client_ptr))
	{
		free(client_ptr);
	}
}

int main(void)
{
	socket_t acceptor, client;

	if (!socket_create(&acceptor))
	{
		fprintf(stderr, "Could not create acceptor");
		return 1;
	}

	if (!socket_bind(acceptor, 8080))
	{
		fprintf(stderr, "Could not bind acceptor");
		socket_destroy(acceptor);
		return 1;
	}

	while (socket_accept(acceptor, &client))
	{
		handle_client(client);
	}

	socket_destroy(acceptor);
	return 0;
}
