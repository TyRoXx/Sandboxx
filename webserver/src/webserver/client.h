#ifndef WS_CLIENT_H
#define WS_CLIENT_H


#include "common/socket.h"
#include "http/directory.h"


typedef struct location_t
{
	char *host;
	directory_t directory;
}
location_t;

void location_destroy(location_t *loc);


typedef struct client_t
{
	socket_t socket;
	location_t const *locations_begin, *locations_end;
}
client_t;


void client_create(client_t *client, socket_t socket, location_t const *locations_begin, location_t const *locations_end);
void client_destroy(client_t *client);
void client_serve(client_t *client);


#endif
