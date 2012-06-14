#include "hash_map.h"
#include <stdio.h>
#include <string.h>


typedef int key_t;
typedef char value_t;

static hash_t hash(const void *key, void *user_data)
{
	key_t original;
	memcpy(&original, key, sizeof(original));
	return original * 1337;
}

static void print_map(hash_map *map)
{
	hash_map_iterator i = hash_map_iterate(map);
	while (hash_map_iterator_next(&i))
	{
		key_t key;
		value_t value;
		memcpy(&key, hash_map_iterator_key(&i), sizeof(key));
		memcpy(&value, hash_map_iterator_value(&i), sizeof(value));
		printf("%d -> %c\n", key, value);
	}
}

int main()
{
	key_t key;
	value_t value;

	hash_map map;
	hash_map_create(&map, sizeof(key_t), sizeof(value_t), hash, 0);

	key = 0;
	value = 'A';
	for (; key < 26; ++key)
	{
		if (!hash_map_insert(&map, &key, &value))
		{
			printf("failed\n");
			break;
		}
		++value;
	}

	for (key = 0; key < 26; key += 2)
	{
		hash_map_erase(&map, &key);
	}
	
	print_map(&map);

	hash_map_destroy(&map);
	return 0;
}
