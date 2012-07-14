#include "socket.h"
#include "thread.h"
#include "http_request.h"
#include <stdio.h>
#include <stdlib.h>


static int receive_char(void *client_ptr)
{
	socket_t client = *(socket_t *)client_ptr;
	char r;
	size_t received;
	if (socket_receive(client, &r, 1, &received))
	{
		return r;
	}
	else
	{
		return -1;
	}
}

static void serve_client(socket_t client)
{
	http_request_t request;

	fprintf(stderr, "Serving client\n");

	if (!http_request_parse(&request, receive_char, &client))
	{
		fprintf(stderr, "Invalid request\n");
		return;
	}

	fprintf(stderr, "%d %s %s\n", request.method, request.host, request.url);

	http_request_destroy(&request);
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
