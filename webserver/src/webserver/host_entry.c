#include "host_entry.h"
#include <stdlib.h>


void host_entry_destroy(host_entry_t *h)
{
	free(h->host);
	directory_destroy(&h->directory);
}
