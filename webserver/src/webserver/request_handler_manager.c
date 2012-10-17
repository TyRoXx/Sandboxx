#include "request_handler_manager.h"
#include <string.h>


void request_handler_manager_create(request_handler_manager_t *m)
{
	WS_GEN_VECTOR_CREATE(m->handlers);
}

void request_handler_manager_destroy(request_handler_manager_t *m)
{
	WS_GEN_VECTOR_DESTROY(m->handlers);
}

loadable_handler_t const *request_handler_manager_find(
	request_handler_manager_t const *m,
	char const *name
	)
{
	loadable_handler_t const *begin = WS_GEN_VECTOR_BEGIN(m->handlers);
	loadable_handler_t const * const end = WS_GEN_VECTOR_END(m->handlers);

	for (; begin != end; ++begin)
	{
		if (!strcmp(begin->name, name))
		{
			return begin;
		}
	}

	return 0;
}
