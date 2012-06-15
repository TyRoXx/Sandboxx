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

struct tree_node
{
	tree_node *parent, *left, *right;
	color_t color;
	char storage[0];
};

static tree_node *allocate_node(size_t storage_size)
{
	tree_node *node = malloc(sizeof(*node) + storage_size);
	return node;
}

static void *node_get_key(tree_node *node)
{
	return node->storage;
}

static void *node_get_value(tree_node *node, size_t key_size)
{
	return ((char *)node->storage) + key_size;
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
	assert(!"to do");
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

			if (relation < 0)
			{
				if (node->left)
				{
					node = node->left;
				}
				else
				{
					assert(!"to do");
				}
			}
			assert(!"to do");
		}
	}
	else
	{
		tree_node *root = allocate_node(map->key_size + map->value_size);
		if (!root)
		{
			return 0;
		}
		root->color = Black;
		root->parent = 0;
		root->left = 0;
		root->right = 0;
		memcpy(node_get_key(root), key, map->key_size);
		memcpy(node_get_value(root, map->key_size), value, map->value_size);
		map->root = root;
		return 1;
	}
}
