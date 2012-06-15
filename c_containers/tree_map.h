#ifndef TREE_MAP_H
#define TREE_MAP_H


#include <stddef.h>


typedef struct tree_node tree_node;

typedef int (*tree_key_comparator_t)(const void *left, const void *right, void *user_data);

typedef struct tree_map
{
	size_t key_size;
	size_t value_size;
	tree_node *root;
	tree_key_comparator_t comparator;
	void *user_data;
}
tree_map;


void tree_map_create(
	tree_map *map,
	size_t key_size,
	size_t value_size,
	tree_key_comparator_t comparator,
	void *user_data);
void tree_map_destroy(tree_map *map);


#endif
