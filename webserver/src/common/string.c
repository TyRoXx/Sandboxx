#include "string.h"
#include <string.h>


bool string_create(string_t *s)
{
	WS_GEN_VECTOR_CREATE(s->data);
	return string_assign_c_str(s, "");
}

void string_destroy(string_t *s)
{
	WS_GEN_VECTOR_DESTROY(s->data);
}

bool string_assign_c_str(string_t *s, char const *source)
{
	return string_assign(s, source, source + strlen(source));
}

bool string_assign(string_t *s, char const *begin, char const *end)
{
	size_t const size = (end - begin);
	WS_GEN_VECTOR_RESIZE(s->data, (size + 1));
	memmove(WS_GEN_VECTOR_DATA(s->data), begin, size);
	WS_GEN_VECTOR_END(s->data)[-1] = '\0';
	return true;
}

void string_clear(string_t *s)
{
	WS_GEN_VECTOR_RESIZE(s->data, 1);
	string_data(s)[0] = '\0';
}

size_t string_size(const string_t *s)
{
	return (WS_GEN_VECTOR_SIZE(s->data) - 1);
}

char const *string_c_str(const string_t *s)
{
	return WS_GEN_VECTOR_DATA(s->data);
}

char *string_data(string_t *s)
{
	return WS_GEN_VECTOR_DATA(s->data);
}
