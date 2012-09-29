#ifndef WS_SETTINGS_H
#define WS_SETTINGS_H


#include "common/generic_vector.h"


typedef struct sub_domain_t
{
	char *name;
	char *destination;
}
sub_domain_t;


void sub_domain_create(sub_domain_t *d, char *name, char *destination);
void sub_domain_destroy(sub_domain_t *d);

typedef struct settings_t
{
	WS_GEN_VECTOR(sub_domains, sub_domain_t);
}
settings_t;


bool settings_create(settings_t *s, const char *begin, const char *end);
void settings_destroy(settings_t *s);


#endif
