#ifndef WS_STRING_H
#define WS_STRING_H


#include "config.h"
#include "generic_vector.h"


typedef struct string_t
{
	WS_GEN_VECTOR(data, char);
}
string_t;


bool string_create(string_t *s);
void string_destroy(string_t *s);
bool string_assign_c_str(string_t *s, char const *source);
bool string_assign(string_t *s, char const *begin, char const *end);
bool string_append_c_str(string_t *s, char const *source);
void string_clear(string_t *s);
size_t string_size(string_t const *s);
char const *string_c_str(string_t const *s);
char *string_data(string_t *s);


#endif
