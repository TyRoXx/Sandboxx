#ifndef WS_SERVER_H
#define WS_SERVER_H


#include "common/socket.h"
#include "common/generic_vector.h"
#include "host_entry.h"
#include "settings.h"


typedef struct server_t
{
	socket_t acceptor;
	WS_GEN_VECTOR(hosts, host_entry_t);
}
server_t;


bool server_create(
	server_t *s,
	settings_t const *settings,
	unsigned port
	);
void server_run(server_t *s);
void server_destroy(server_t *s);


#endif
