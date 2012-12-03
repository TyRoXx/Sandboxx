#ifndef SIGNALS_CONNECTION_H
#define SIGNALS_CONNECTION_H


#include "types.h"
#include <stddef.h>


struct connection
{
	slot callback;
	void *user_data;
	connection *previous, *next;
	size_t external_refs;
	signal *parent;
	int is_connected;
};

void connection_remove(connection *c);
void connection_remove_if_disconnected(connection *c);
void connection_destroy(connection *c);
void connection_grab(connection *c);
void connection_drop(connection *c);
int connection_is_connected(connection const *c);
void connection_disconnect(connection *c);


#endif
