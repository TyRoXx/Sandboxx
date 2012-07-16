#include "http_response.h"
#include <string.h>
#include <stdlib.h>


const char *http_status_message(http_status_t status)
{
	switch (status)
	{
	case Status_Ok: return "OK";
	case Status_NotFound: return "Not Found";
	}
	return "Unknown Status";
}


void http_header_destroy(http_header_t *header)
{
	free(header->key);
	free(header->value);
}

void http_response_destroy(http_response_t *response)
{
	size_t i;

	for (i = 0; i < response->header_count; ++i)
	{
		http_header_destroy(response->headers + i);
	}
	buffer_destroy(&response->body);
}

http_header_t *http_response_header(http_response_t *response, const char *key)
{
	size_t i;

	for (i = 0; i < response->header_count; ++i)
	{
		http_header_t * const header = response->headers + i;
		if (!strcmp(header->key, key))
		{
			return header;
		}
	}
	return 0;
}
