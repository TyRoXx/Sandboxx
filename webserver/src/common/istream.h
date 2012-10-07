#ifndef WS_ISTREAM_H
#define WS_ISTREAM_H


#include "config.h"
#include <stddef.h>


typedef struct istream_t
{
	void (*fetch)(struct istream_t *);
	void (*discard)(struct istream_t *, char const *);
	void (*destroy)(struct istream_t *);
	void *impl;
	char const *begin, *end;
}
istream_t;


void istream_create(
	istream_t *this,
	void (*fetch)(istream_t *),
	void (*discard)(istream_t *, char const *),
	void (*destroy)(istream_t *),
	void *impl
	);
void istream_destroy(istream_t *this);
void istream_fetch(istream_t *this);
void istream_discard(istream_t *this, char const *until);
void istream_discard_all(istream_t *this);
bool istream_empty(istream_t *this);
char const *istream_data(istream_t *this);
size_t istream_size(istream_t *this);


#endif
