#ifndef WS_SERVER_H
#define WS_SERVER_H


#include "common/socket.h"
#include "common/generic_vector.h"
#include "host_entry.h"
#include "settings.h"
#include "node_plugin_manager.h"
#include "request_handler_manager.h"
#include "log.h"


typedef struct server_t
{
	log_t *log;
	socket_t acceptor;
	node_plugin_manager_t plugins;
	request_handler_manager_t request_handlers;
	WS_GEN_VECTOR(hosts, host_entry_t);
}
server_t;


bool server_create(
	server_t *s,
	log_t *log,
	settings_t const *settings,
	unsigned port
	);
void server_run(server_t *s);
void server_destroy(server_t *s);


#endif
