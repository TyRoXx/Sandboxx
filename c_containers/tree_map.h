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
int tree_map_insert(tree_map *map, const void *key, const void *value);
void tree_map_erase(tree_map *map, const void *key);
const void *tree_map_find(tree_map *map, const void *key);
void tree_map_clear(tree_map *map);


#endif
