#ifndef TREE_MAP_H
#define TREE_MAP_H


#include <stddef.h>


typedef struct tree_node tree_node;

typedef struct tree_map
{
	size_t value_size;
	tree_node *root;
}
tree_map;


void tree_map_create(tree_map *map, size_t value_size);
void tree_map_destroy(tree_map *map);


#endif
