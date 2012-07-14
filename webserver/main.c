#include "socket.h"
#include "thread.h"
#include "http_request.h"
#include "http_response.h"
#include "lua_request_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static bool load_buffer_from_file_name(buffer_t *content, const char *file_name)
{
	static const size_t Growth = 4096;
	size_t total_read = 0;
	FILE *file = fopen(file_name, "rb");
	if (!file)
	{
		return false;
	}

	for (;;)
	{
		if (!buffer_resize(content, total_read + Growth))
		{
			fclose(file);
			return 0;
		}
		const size_t just_read = fread(content->data + total_read, 1, Growth, file);
		if (just_read == 0)
		{
			break;
		}
		total_read += just_read;
	}

	buffer_resize(content, total_read);
	fclose(file);
	return true;
}

static void handle_request(socket_t client, const http_request_t *request)
{
	http_response_t response = {0};
	lua_handler_context_t lua_context;
	buffer_create(&lua_context.script);
	if (!load_buffer_from_file_name(&lua_context.script, "index.lua"))
	{
		fprintf(stderr, "Could not load script file\n");
		return;
	}

	if (handle_lua_request(request, &response, &lua_context))
	{
		fprintf(stderr, "Sending response\n");

		char header[8192];
		sprintf(header,
			"HTTP/1.1 OK 200\r\n"
			"Content-Length: %u\r\n"
			"Content-Type: text/html\r\n"
			"Connection: close\r\n"
			"\r\n",
			(unsigned)response.body.size);

		socket_send(client, header, strlen(header));
		socket_send(client, response.body.data, response.body.size);
	}

	buffer_destroy(&lua_context.script);
}

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

static void receive_request(socket_t client)
{
	http_request_t request;

	fprintf(stderr, "Serving client\n");

	if (!http_request_parse(&request, receive_char, &client))
	{
		fprintf(stderr, "Invalid request\n");
		return;
	}

	fprintf(stderr, "%d %s %s\n", request.method, request.host, request.url);
	handle_request(client, &request);

	http_request_destroy(&request);
}

static void wait_for_disconnect(socket_t client)
{
	char c;
	size_t received;

	while (socket_receive(client, &c, 1, &received))
	{
	}
}

static void serve_client(socket_t client)
{
	receive_request(client);
	wait_for_disconnect(client);
	socket_destroy(client);
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

	if (!socket_bind(acceptor, 8081))
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
