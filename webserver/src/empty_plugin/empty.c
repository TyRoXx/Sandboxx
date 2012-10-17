#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>


#ifdef _WIN32
#define WS_EXPORT __declspec(dllexport)
#else
#define WS_EXPORT
#endif


static size_t min_size_t(size_t first, size_t second)
{
	return (first < second) ? first : second;
}

WS_EXPORT
void *np_realloc(void *data, size_t size)
{
	return realloc(data, size);
}

WS_EXPORT
void np_free(void *data)
{
	free(data);
}

WS_EXPORT
void np_get_info(char *out_name, size_t name_max, unsigned *api_version)
{
	char const * const name = "plugin_empty";
	size_t const length = min_size_t(name_max, strlen(name));

	memcpy(out_name, name, length);
	out_name[length] = '\0';

	*api_version = 0;
}

WS_EXPORT
int np_handle_request(
	char const *method,
	char const *url,
	char const *host,
	char const * const *request_headers,
	char const *request_body,
	size_t request_body_size,
	char **response_headers,
	char **response_body,
	size_t *response_body_size)
{
	*response_body = np_realloc(0, 100);
	if (!*response_body)
	{
		return 0;
	}

	sprintf(*response_body, "hello, world!\n%u", (unsigned)time(0));
	*response_body_size = strlen(*response_body);
	return 1;
}
