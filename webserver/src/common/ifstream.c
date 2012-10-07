#include "ifstream.h"
#include "buffer.h"
#include <stdlib.h>
#include <assert.h>


typedef struct ifstream_impl_t
{
	FILE *file;
	buffer_t buffer;
}
ifstream_impl_t;

static void fetch(istream_t *this)
{
	static const size_t ReadSize = 4096;

	ifstream_impl_t * const impl = this->impl;
	buffer_t * const buffer = &impl->buffer;
	FILE * const file = impl->file;
	size_t const old_buffer_size = buffer->size;
	size_t read;

	if (!buffer_resize(buffer, old_buffer_size + ReadSize))
	{
		return;
	}

	read = fread(buffer->data + old_buffer_size, 1, ReadSize, file);
	assert(read <= ReadSize);

	if (!buffer_resize(buffer, old_buffer_size + read))
	{
		assert(!"Should not fail");
		return;
	}

	this->begin = buffer->data;
	this->end = this->begin + buffer->size;
}

static void discard(istream_t *this, char const *until)
{
	ifstream_impl_t * const impl = this->impl;
	buffer_t * const buffer = &impl->buffer;

	buffer_erase(buffer, buffer->data, (char *) until);
}

static void destroy(istream_t *this)
{
	ifstream_impl_t * const impl = this->impl;
	buffer_t * const buffer = &impl->buffer;

	buffer_destroy(buffer);
	free(impl);
}

bool ifstream_create(
	istream_t *this,
	FILE *file
	)
{
	ifstream_impl_t * const impl = malloc(sizeof(*impl));
	if (!impl)
	{
		return false;
	}

	impl->file = file;
	buffer_create(&impl->buffer);

	istream_create(this, fetch, discard, destroy, impl);
	return true;
}
