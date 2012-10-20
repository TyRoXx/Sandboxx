#include "client.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "common/string_util.h"
#include <stdio.h>
#include <stdlib.h>


static void client_log(client_t *client, char const *format, ...)
{
	log_t * const log = client->log;
	FILE * const out = log_get_out(log);
	mutex_t * const out_mutex = log_get_out_mutex(log);

	va_list args;
	va_start(args, format);
	mutex_lock(out_mutex);

	fprintf(out, "%s: ", client->name);
	vfprintf(out, format, args);
	fputs("\n", out);

	mutex_unlock(out_mutex);
	va_end(args);
}

static const directory_t *find_directory_by_host(const client_t *client, char const *host)
{
	const host_entry_t *loc, *result = 0;
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

static void handle_request(client_t *client, const http_request_t *request)
{
	http_response_t response = {0};
	char const *url = request->url;
	const directory_t * const directory = find_directory_by_host(client, request->host);

	if (!directory)
	{
		/* TODO error handling */
		client_log(client, "No directory for host '%s'", request->host);
		return;
	}

	if (*url == '/')
	{
		++url;
	}

	response.status = HttpStatus_Ok;

	if (!string_create(&response.headers))
	{
		return;
	}

	if (directory_handle_request(directory, url, &response))
	{
		char buffer[8192];
		char const * const status_message = http_status_message(response.status);
		bool send_failed = false;

		client_log(client, "Sending response");

		sprintf(buffer,
			"HTTP/1.1 %s %u\r\n"
			"Content-Length: %u\r\n"
			"Connection: close\r\n"
			,
			status_message,
			(unsigned)response.status,
			(unsigned)response.body_size);

		if ((send_failed = !socket_send(client->socket, buffer, strlen(buffer))))
		{
			goto send_ended;
		}

		if ((send_failed = !socket_send(
			client->socket,
			string_data(&response.headers),
			string_size(&response.headers))))
		{
			goto send_ended;
		}

		if (!socket_send(client->socket, "\r\n", 2) ||
			!send_istream(client->socket, &response.body))
		{
			send_failed = true;
		}

send_ended:
		if (send_failed)
		{
			client_log(client, "Send failed");
		}
	}

	http_response_destroy(&response);
}

static int receive_char(void *client_ptr)
{
	socket_t const client = *(socket_t *)client_ptr;
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

	client_log(client, "Serving client");

	if (!http_request_parse(&request, receive_char, &client->socket))
	{
		client_log(client, "Invalid request");
		return;
	}

	client_log(client, "%s %s %s", request.method, request.host, request.url);

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


void client_create(
	client_t *client,
	socket_t socket,
	host_entry_t const *locations_begin,
	host_entry_t const *locations_end,
	char *name,
	log_t *log
	)
{
	client->socket = socket;
	client->locations_begin = locations_begin;
	client->locations_end = locations_end;
	client->name = name;
	client->log = log;
}

void client_destroy(client_t *client)
{
	socket_destroy(client->socket);
	free(client->name);
}

void client_serve(client_t *client)
{
	receive_request(client);
	socket_shutdown(client->socket);
	wait_for_disconnect(client->socket);

	client_log(client, "Disconnected");
}
