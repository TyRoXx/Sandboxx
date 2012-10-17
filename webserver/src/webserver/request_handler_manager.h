#ifndef WS_REQUEST_HANDLER_MANAGER_H
#define WS_REQUEST_HANDLER_MANAGER_H


#include "common/generic_vector.h"
#include "http/load_directory.h"


typedef struct request_handler_manager_t
{
	WS_GEN_VECTOR(handlers, loadable_handler_t);
}
request_handler_manager_t;


void request_handler_manager_create(request_handler_manager_t *m);
void request_handler_manager_destroy(request_handler_manager_t *m);
loadable_handler_t const *request_handler_manager_find(
	request_handler_manager_t const *m,
	char const *name
	);


#endif
