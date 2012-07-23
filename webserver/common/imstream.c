#include "imstream.h"
#include <stdlib.h>


typedef struct imstream_impl_t
{
	const char *data;
	size_t size;
}
imstream_impl_t;

static void fetch(istream_t *this)
{
	imstream_impl_t * const impl = this->impl;
	const char *end = impl->data + impl->size;

	if (this->end != end)
	{
		this->begin = impl->data;
		this->end = end;
	}
}

static void discard(istream_t *this, const char *until)
{
	this->begin = until;
}

static void destroy(istream_t *this)
{
	free(this->impl);
}

bool imstream_create(
	istream_t *this,
	const void *data,
	size_t size
	)
{
	imstream_impl_t * const impl = malloc(sizeof(*impl));
	if (!impl)
	{
		return false;
	}

	impl->data = data;
	impl->size = size;

	istream_create(this, fetch, discard, destroy, impl);
	return true;
}