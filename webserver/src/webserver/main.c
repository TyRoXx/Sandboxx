#include "common/socket.h"
#include "common/thread.h"
#include "common/path.h"
#include "common/string_util.h"
#include "common/load_file.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "http/directory.h"
#include "http/load_directory.h"
#include "lua_script/lua_script.h"
#include "file_system/fs_directory.h"
#include "sub_directory/sub_directory.h"
#include "settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef WEBSERVER_WITH_VLD
#include <vld.h>
#endif


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


typedef struct location_t
{
	char *host;
	directory_t directory;
}
location_t;

static void location_destroy(location_t *loc)
{
	free(loc->host);
	directory_destroy(&loc->directory);
}


typedef struct client_t
{
	socket_t socket;
	const location_t *locations_begin, *locations_end;
}
client_t;


static const directory_t *find_directory_by_host(const client_t *client, const char *host)
{
	const location_t *loc, *result = 0;
	const size_t significant_host_length = string_index_of(host, ':');
	
	for (loc = client->locations_begin; loc != client->locations_end; ++loc)
	{
		if (!strncmp(loc->host, host, significant_host_length))
		{
			result = loc;
			break;
		}

		if (*loc->host == '\0')
		{
			result = loc;
		}
	}

	return (result ? &result->directory : 0);
}

static void handle_request(client_t *client, const http_request_t *request)
{
	http_response_t response = {0};
	const char *url = request->url;
	const directory_t * const directory = find_directory_by_host(client, request->host);

	if (!directory)
	{
		/* TODO error handling */
		fprintf(stderr, "No directory for host '%s'\n", request->host);
		return;
	}

	if (*url == '/')
	{
		++url;
	}

	response.status = HttpStatus_Ok;

	if (directory_handle_request(directory, url, &response))
	{
		char buffer[8192];
		size_t i;
		const char * const status_message = http_status_message(response.status);
		bool send_failed = false;

		fprintf(stderr, "Sending response\n");

		sprintf(buffer,
			"HTTP/1.1 %s %d\r\n"
			"Content-Length: %u\r\n"
			"Connection: close\r\n"
			,
			status_message,
			(int)response.status,
			(unsigned)response.body_size);

		if ((send_failed = !socket_send(client->socket, buffer, strlen(buffer))))
		{
			goto send_ended;
		}

		for (i = 0; i < response.header_count; ++i)
		{
			const http_header_t *header = response.headers + i;
			sprintf(buffer,
				"%s: %s\r\n",
				header->key,
				header->value);

			if ((send_failed = !socket_send(client->socket, buffer, strlen(buffer))))
			{
				goto send_ended;
			}
		}

		if (!socket_send(client->socket, "\r\n", 2) ||
			!send_istream(client->socket, &response.body))
		{
			send_failed = true;
		}

send_ended:
		if (send_failed)
		{
			fprintf(stderr, "Send failed\n");
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

static void receive_request(client_t *client)
{
	http_request_t request;

	fprintf(stderr, "Serving client\n");

	if (!http_request_parse(&request, receive_char, &client->socket))
	{
		fprintf(stderr, "Invalid request\n");
		return;
	}

	fprintf(stderr, "%d %s %s\n", request.method, request.host, request.url);
	handle_request(client, &request);

	http_request_destroy(&request);
}

static void wait_for_disconnect(socket_t s)
{
	char c;
	size_t received;

	while (socket_receive(s, &c, 1, &received))
	{
	}
}

static void serve_client(client_t *client)
{
	receive_request(client);
	socket_shutdown(client->socket);
	wait_for_disconnect(client->socket);
}

static void client_thread_proc(void *client_ptr)
{
	client_t * const client = client_ptr;

	serve_client(client);
	socket_destroy(client->socket);

	free(client);

	thread_quit();
}

static void handle_client(
	socket_t s,
	const location_t *locations_begin,
	const location_t *locations_end)
{
	thread_t client_thread;
	client_t * const client = malloc(sizeof(*client));

	if (!client)
	{
		socket_destroy(s);
		return;
	}

	client->socket = s;
	client->locations_begin = locations_begin;
	client->locations_end = locations_end;

	if (!thread_create(&client_thread, client_thread_proc, client))
	{
		socket_destroy(s);
		free(client);
	}
}

static bool load_location(location_t *loc, const char *host, const char *path)
{
	static const loadable_handler_t handlers[] =
	{
		{"lua", initialize_lua_script},
		{"fs", initialize_file_system},
		{"dir", initialize_sub_directory},
	};

	char * const directory_file_name = path_join(path, "directory.txt");
	buffer_t dir_file;

	assert(loc);
	assert(host);
	assert(path);

	buffer_create(&dir_file);

	if (!load_buffer_from_file_name(&dir_file, directory_file_name))
	{
		fprintf(stderr, "Could not load directory file '%s'\n", directory_file_name);
		buffer_destroy(&dir_file);
		free(directory_file_name);
		return false;
	}

	free(directory_file_name);

	directory_create(&loc->directory);

	if (!load_directory(
		&loc->directory,
		dir_file.data,
		dir_file.data + dir_file.size,
		handlers,
		handlers + sizeof(handlers) / sizeof(handlers[0]),
		path))
	{
		fprintf(stderr, "Could not parse directory file\n");
		buffer_destroy(&dir_file);
		return false;
	}

	buffer_destroy(&dir_file);

	loc->host = string_duplicate(host);
	return (loc->host != 0);
}

static void destroy_locations(location_t *locations_begin, location_t *locations_end)
{
	for (; locations_begin != locations_end; ++locations_begin)
	{
		location_destroy(locations_begin);
	}
}

int main(int argc, char **argv)
{
	const unsigned short acceptor_port = ((argc >= 2) ? (unsigned short)atoi(argv[1]) : 8080);
	const char * const settings_file_name = ((argc >= 3) ? argv[2] : "settings.txt");
	socket_t acceptor, client;
	location_t *locations_begin, *locations_end, *loc;
	settings_t settings;
	buffer_t settings_content;
	host_entry_t *host;
	int result;

	buffer_create(&settings_content);

	if (!load_buffer_from_file_name(&settings_content, settings_file_name))
	{
		fprintf(stderr, "Could not load settings file '%s'\n", settings_file_name);
		return 1;
	}

	if (!settings_create(&settings, settings_content.data, settings_content.data + settings_content.size))
	{
		buffer_destroy(&settings_content);
		return 1;
	}

	buffer_destroy(&settings_content);

	locations_begin = loc = malloc(sizeof(*locations_begin) * WS_GEN_VECTOR_SIZE(settings.hosts));
	locations_end = locations_begin;

	for (host = WS_GEN_VECTOR_BEGIN(settings.hosts);
		host != WS_GEN_VECTOR_END(settings.hosts);
		++host, ++loc, ++locations_end)
	{
		if (!load_location(loc, host->name, host->destination))
		{
			destroy_locations(locations_begin, locations_end);
			free(locations_begin);
			settings_destroy(&settings);
			return 1;
		}
	}

	settings_destroy(&settings);

	if (!socket_create(&acceptor))
	{
		fprintf(stderr, "Could not create acceptor\n");
		result = 1;
		goto cleanup;
	}

	if (!socket_bind(acceptor, acceptor_port))
	{
		fprintf(stderr, "Could not bind acceptor to port %u\n", (unsigned)acceptor_port);
		result = 1;
		goto cleanup;
	}

	while (socket_accept(acceptor, &client))
	{
		handle_client(
			client,
			locations_begin,
			locations_end);
	}

	result = 0;

cleanup:
	socket_destroy(acceptor);

	destroy_locations(locations_begin, locations_end);
	free(locations_begin);
	return result;
}
