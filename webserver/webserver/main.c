#include "common/socket.h"
#include "common/thread.h"
#include "common/load_file.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "http/directory.h"
#include "http/load_directory.h"
#include "lua_script/lua_script.h"
#include "file_system/fs_directory.h"
#include "sub_directory/sub_directory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WEBSERVER_WITH_VLD
#include <vld.h>
#endif


static directory_t top_dir;


static bool send_istream(socket_t receiver, istream_t *source)
{
	for (;;)
	{
		istream_fetch(source);

		if (istream_empty(source))
		{
			return true;
		}

		if (!socket_send(
			receiver,
			istream_data(source),
			istream_size(source)))
		{
			return false;
		}

		istream_discard_all(source);
	}
}

static void handle_request(socket_t client, const http_request_t *request)
{
	http_response_t response = {0};
	const char *url = request->url;

	if (*url == '/')
	{
		++url;
	}

	response.status = HttpStatus_Ok;
	
	if (directory_handle_request(&top_dir, url, &response))
	{
		char buffer[8192];
		size_t i;
		const char * const status_message = http_status_message(response.status);

		fprintf(stderr, "Sending response\n");

		sprintf(buffer,
			"HTTP/1.1 %s %d\r\n"
			"Content-Length: %u\r\n"
			"Connection: close\r\n"
			,
			status_message,
			(int)response.status,
			(unsigned)response.body_size);

		socket_send(client, buffer, strlen(buffer));

		for (i = 0; i < response.header_count; ++i)
		{
			const http_header_t *header = response.headers + i;
			sprintf(buffer,
				"%s: %s\r\n",
				header->key,
				header->value);

			socket_send(client, buffer, strlen(buffer));
		}

		socket_send(client, "\r\n", 2);
		if (!send_istream(client, &response.body))
		{
			fprintf(stderr, "Connection closed by client\n");
		}
	}

	http_response_destroy(&response);
}

static int receive_char(void *client_ptr)
{
	const socket_t client = *(socket_t *)client_ptr;
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
	socket_t * const client_ptr = malloc(sizeof(*client_ptr));

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

	static const loadable_handler_t handlers[] =
	{
		{"lua", initialize_lua_script},
		{"fs", initialize_file_system},
		{"dir", initialize_sub_directory},
	};

	buffer_t dir_file;
	buffer_create(&dir_file);

	if (!load_buffer_from_file_name(&dir_file, "directory.txt"))
	{
		fprintf(stderr, "Could not load directory file\n");
		buffer_destroy(&dir_file);
		return 1;
	}

	directory_create(&top_dir);

	if (!load_directory(&top_dir,
		dir_file.data,
		dir_file.data + dir_file.size,
		handlers,
		handlers + sizeof(handlers) / sizeof(handlers[0]),
		"."))
	{
		fprintf(stderr, "Could not parse directory file\n");
		buffer_destroy(&dir_file);
		return 1;
	}

	buffer_destroy(&dir_file);

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
	directory_destroy(&top_dir);
	return 0;
}
