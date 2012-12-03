#ifndef SIGNALS_SIGNAL_H
#define SIGNALS_SIGNAL_H


#include "types.h"
#include <stddef.h>


struct signal
{
	connection *first, *last;
	size_t call_depth;
};

void signal_create(signal *s);
void signal_destroy(signal *s);
connection *signal_connect(signal *s, slot callback, void *user_data);
void signal_disconnect(signal *s, connection *c);
void signal_call(signal *s, void *arguments);


#endif
