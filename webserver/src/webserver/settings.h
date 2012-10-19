#ifndef WS_SETTINGS_H
#define WS_SETTINGS_H


#include "common/generic_vector.h"


typedef struct settings_host_entry_t
{
	char *name;
	char *destination;
}
settings_host_entry_t;


void settings_host_entry_create(settings_host_entry_t *d, char *name, char *destination);
void settings_host_entry_destroy(settings_host_entry_t *d);

typedef struct settings_t
{
	WS_GEN_VECTOR(hosts, settings_host_entry_t);
	WS_GEN_VECTOR(plugin_file_names, char *);
}
settings_t;


bool settings_create(settings_t *s, char const *begin, char const *end);
void settings_destroy(settings_t *s);


#endif
