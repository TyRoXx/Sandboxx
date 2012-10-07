#ifndef WS_SETTINGS_H
#define WS_SETTINGS_H


#include "common/generic_vector.h"


typedef struct host_entry_t
{
	char *name;
	char *destination;
}
host_entry_t;


void host_entry_create(host_entry_t *d, char *name, char *destination);
void host_entry_destroy(host_entry_t *d);

typedef struct settings_t
{
	WS_GEN_VECTOR(hosts, host_entry_t);
}
settings_t;


bool settings_create(settings_t *s, char const *begin, char const *end);
void settings_destroy(settings_t *s);


#endif
