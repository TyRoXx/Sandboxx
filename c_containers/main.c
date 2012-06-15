#include "hash_map.h"
#include "hash_set.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>


typedef long long key_t;
typedef char value_t;

static hash_t hash(const void *key, void *user_data)
{
	key_t original;
	memcpy(&original, key, sizeof(original));
	return (hash_t)(original * 1337);
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
		printf("%d -> %c\n", (int)key, (char)value);
	}
}

static void print_set(hash_set *set)
{
	hash_set_iterator i = hash_set_iterate(set);
	while (hash_set_iterator_next(&i))
	{
		key_t key;
		memcpy(&key, hash_set_iterator_key(&i), sizeof(key));
		printf("%d\n", (int)key);
	}
}

int main()
{
	key_t key;
	value_t value;

	hash_map map;
	hash_set set;
	hash_map_create(&map, sizeof(key_t), sizeof(value_t), hash, 0);
	hash_set_create(&set, sizeof(key_t), hash, 0);

	key = 0;
	value = 'A';
	for (; key < 26; ++key)
	{
		if (!hash_map_insert(&map, &key, &value) ||
			!hash_set_insert(&set, &key))
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

	assert(hash_map_size(&map) == 13);
	assert(hash_set_size(&set) == 26);
	
	print_map(&map);
	printf("\n");
	print_set(&set);

	hash_map_destroy(&map);
	return 0;
}
