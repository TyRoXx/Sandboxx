#include "tree_map.h"
#include <assert.h>


void tree_map_create(tree_map *map, size_t value_size)
{
	map->root = 0;
	map->value_size = value_size;
}

void tree_map_destroy(tree_map *map)
{
	assert(!"to do");
}
