#include "tree_map.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>


typedef enum color_t
{
	Red,
	Black,
}
color_t;

typedef enum child_t
{
	Left,
	Right,
}
child_t;

static child_t relationToChild(int relation)
{
	assert(relation != 0);
	return (relation < 0) ? Left : Right;
}

struct tree_node
{
	tree_node *parent;
	tree_node *children[2];
	color_t color;
	char storage[0];
};

static tree_node *allocate_node(const tree_map *map)
{
	tree_node *node = malloc(sizeof(*node) + map->key_size + map->value_size);
	return node;
}

static void deallocate_node(tree_node *node)
{
	free(node);
}

static void destroy_nodes(tree_node *root)
{
	size_t i;
	for (i = 0; i < 2; ++i)
	{
		tree_node *child = root->children[i];
		if (child)
		{
			destroy_nodes(child);
		}
	}
	deallocate_node(root);
}

static void *node_get_key(tree_node *node)
{
	return node->storage;
}

static void *node_get_value(tree_node *node, const tree_map *map)
{
	return ((char *)node->storage) + map->key_size;
}

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
	if (map->root)
	{
		destroy_nodes(map->root);
	}
}

int tree_map_insert(tree_map *map, const void *key, const void *value)
{
	if (map->root)
	{
		tree_node *node = map->root;
		for (;;)
		{
			int relation = map->comparator(
				key,
				node_get_key(node),
				map->user_data
				);

			if (relation == 0)
			{
				memmove(
					node_get_value(node, map),
					value,
					map->value_size
					);
				break;
			}
			else
			{
				const child_t childId = relationToChild(relation);
				tree_node *child = node->children[childId];
				if (child)
				{
					node = child;
				}
				else
				{
					child = allocate_node(map);;
					if (!child)
					{
						return 0;
					}
					child->parent = node;
					child->color = Red;
					child->children[Left] = 0;
					child->children[Right] = 0;
					memmove(node_get_key(child), key, map->key_size);
					memmove(node_get_value(child, map), value, map->value_size);

					node->children[childId] = child;
					break;
				}
			}
		}

		return 1;
	}
	else
	{
		tree_node *root = allocate_node(map);
		if (!root)
		{
			return 0;
		}
		root->color = Black;
		root->parent = 0;
		root->children[Left] = 0;
		root->children[Right] = 0;
		memcpy(node_get_key(root), key, map->key_size);
		memcpy(node_get_value(root, map), value, map->value_size);
		map->root = root;
		return 1;
	}
}

static tree_node *find_node(tree_node *node, tree_map *map, const void *key)
{
	assert(node);
	assert(map);
	assert(key);

	do
	{
		int relation = map->comparator(
			key,
			node_get_key(node),
			map->user_data
			);

		if (relation == 0)
		{
			break;
		}
		else
		{
			node = node->children[relationToChild(relation)];
		}
	}
	while (node);
	return node;
}

void *tree_map_find(tree_map *map, const void *key)
{
	tree_node *root = map->root;
	tree_node *found;
	if (!root)
	{
		return 0;
	}
	found = find_node(root, map, key);
	return found ?
		node_get_value(found, map) :
		0;
}

void tree_map_clear(tree_map *map)
{
	size_t key_size = map->key_size;
	size_t value_size = map->value_size;
	tree_key_comparator_t comparator = map->comparator;
	void *user_data = map->user_data;

	tree_map_destroy(map);
	tree_map_create(map, key_size, value_size, comparator, user_data);
}
