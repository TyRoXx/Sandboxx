#include "map.h"
#include <stdio.h>


int main(void)
{
	map_t *m = map_create();
	map_insert(m, "abc", "123");
	printf("%u\n", (unsigned)map_size(m));
	printf("%s\n", map_find(m, "abc"));
	map_destroy(m);
	return 0;
}
