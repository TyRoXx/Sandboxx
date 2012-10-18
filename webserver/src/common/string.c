#include "string.h"
#include <string.h>
#include <assert.h>


bool string_create(string_t *s)
{
	assert(s);

	WS_GEN_VECTOR_CREATE(s->data);
	return string_assign_c_str(s, "");
}

void string_destroy(string_t *s)
{
	assert(s);

	WS_GEN_VECTOR_DESTROY(s->data);
}

bool string_assign_c_str(string_t *s, char const *source)
{
	assert(s);
	assert(source);

	return string_assign(s, source, source + strlen(source));
}

bool string_assign(string_t *s, char const *begin, char const *end)
{
	size_t const size = (end - begin);
	bool success;

	assert(s);
	assert(begin);
	assert(end);

	WS_GEN_VECTOR_RESIZE(s->data, (size + 1), success);
	if (!success)
	{
		return false;
	}

	memmove(WS_GEN_VECTOR_DATA(s->data), begin, size);
	WS_GEN_VECTOR_END(s->data)[-1] = '\0';
	return true;
}

bool string_append_c_str(string_t *s, char const *source)
{
	size_t const source_length = strlen(source);
	size_t const old_size = string_size(s);
	bool success;

	WS_GEN_VECTOR_RESIZE(
		s->data,
		WS_GEN_VECTOR_SIZE(s->data) + source_length,
		success);

	if (!success)
	{
		return false;
	}

	memmove(
		WS_GEN_VECTOR_DATA(s->data) + old_size,
		source,
		source_length + 1
		);
	return true;
}

void string_clear(string_t *s)
{
	bool success;
	assert(s);

	WS_GEN_VECTOR_RESIZE(s->data, 1, success);
	assert(success);

	string_data(s)[0] = '\0';
}

size_t string_size(string_t const *s)
{
	assert(s);
	return (WS_GEN_VECTOR_SIZE(s->data) - 1);
}

char const *string_c_str(string_t const *s)
{
	assert(s);
	return WS_GEN_VECTOR_DATA(s->data);
}

char *string_data(string_t *s)
{
	assert(s);
	return WS_GEN_VECTOR_DATA(s->data);
}
