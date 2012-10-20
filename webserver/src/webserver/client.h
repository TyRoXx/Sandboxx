#ifndef WS_CLIENT_H
#define WS_CLIENT_H


#include "common/socket.h"
#include "http/directory.h"
#include "host_entry.h"
#include "log.h"


typedef struct client_t
{
	socket_t socket;
	host_entry_t const *locations_begin, *locations_end;
	char *name;
	log_t *log;
}
client_t;


void client_create(
	client_t *client,
	socket_t socket,
	host_entry_t const *locations_begin,
	host_entry_t const *locations_end,
	char *name,
	log_t *log
	);
void client_destroy(client_t *client);
void client_serve(client_t *client);


#endif
