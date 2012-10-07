#include "istream.h"


void istream_create(
	istream_t *this,
	void (*fetch)(istream_t *),
	void (*discard)(istream_t *, char const *),
	void (*destroy)(istream_t *),
	void *impl
	)
{
	this->fetch = fetch;
	this->discard = discard;
	this->destroy = destroy;
	this->impl = impl;
	this->begin = this->end = 0;
}

void istream_destroy(istream_t *this)
{
	if (this->destroy)
	{
		this->destroy(this);
	}
}

void istream_fetch(istream_t *this)
{
	this->fetch(this);
}

void istream_discard(istream_t *this, char const *until)
{
	this->discard(this, until);
}

void istream_discard_all(istream_t *this)
{
	istream_discard(this, this->end);
}

bool istream_empty(istream_t *this)
{
	return (this->begin == this->end);
}

char const *istream_data(istream_t *this)
{
	return this->begin;
}

size_t istream_size(istream_t *this)
{
	return (this->end - this->begin);
}
