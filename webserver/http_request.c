#include "http_request.h"
#include <stdlib.h>


bool http_request_parse(
	http_request_t *request,
	int (*read)(void *),
	void *data)
{
	return false;
}

static void destroy_header(http_header_t *header)
{
	free(header->key);
	free(header->value);
}

void http_request_destroy(
	http_request_t *request)
{
	size_t i;

	for (i = 0; i < request->header_count; ++i)
	{
		destroy_header(request->headers + i);
	}

	free(request->path);
	free(request->search);
}
