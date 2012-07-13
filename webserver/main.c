#include "socket.h"
#include "thread.h"
#include "http_request.h"
#include <stdio.h>
#include <stdlib.h>


static void serve_client(socket_t client)
{
	fprintf(stdout, "Serving client\n");


}

static void client_thread_proc(void *client_ptr)
{
	socket_t client = *(socket_t *)client_ptr;
	free(client_ptr);
	serve_client(client);

	socket_destroy(client);
	thread_quit();
}

static void handle_client(socket_t client)
{
	thread_t client_thread;
	socket_t *client_ptr = malloc(sizeof(*client_ptr));
	if (!client_ptr)
	{
		socket_destroy(client);
		return;
	}
	*client_ptr = client;
	if (!thread_create(&client_thread, client_thread_proc, client_ptr))
	{
		free(client_ptr);
		socket_destroy(client);
	}
}

int main(void)
{
	socket_t acceptor, client;

	if (!socket_create(&acceptor))
	{
		fprintf(stderr, "Could not create acceptor\n");
		return 1;
	}

	if (!socket_bind(acceptor, 8080))
	{
		fprintf(stderr, "Could not bind acceptor\n");
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
