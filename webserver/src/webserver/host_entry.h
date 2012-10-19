#ifndef WS_HOST_ENTRY_H
#define WS_HOST_ENTRY_H


#include "http/directory.h"


typedef struct host_entry_t
{
	char *host;
	directory_t directory;
}
host_entry_t;

void host_entry_destroy(host_entry_t *h);


#endif
