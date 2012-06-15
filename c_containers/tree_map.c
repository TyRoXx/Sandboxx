#include "tree_map.h"
#include <assert.h>


void tree_map_create(
	tree_map *map,
	size_t key_size,
	size_t value_size,
	tree_key_comparator_t comparator,
	void *user_data)
{
	map->key_size = key_size;
	map->value_size = value_size;
	map->root = 0;
	map->comparator = comparator;
	map->user_data = user_data;
}

void tree_map_destroy(tree_map *map)
{
	assert(!"to do");
}
