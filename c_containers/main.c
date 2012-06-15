#include "hash_map.h"
#include "hash_set.h"
#include "queue.h"
#include "linked_list.h"
#include "vector.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>


#define ENSURE(x) { const int s = (x); assert(s); }

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

static void test_hash_map()
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
		ENSURE(hash_map_insert(&map, &key, &value));
		ENSURE(hash_set_insert(&set, &key));

		++value;
	}

	for (key = 0; key < 26; key += 2)
	{
		hash_map_erase(&map, &key);
	}

	ENSURE(hash_map_size(&map) == 13);
	ENSURE(hash_set_size(&set) == 26);

	print_map(&map);
	printf("\n");
	print_set(&set);

	hash_map_destroy(&map);
}

static void test_vector()
{
	typedef unsigned element_t;

	element_t e;
	vector_iterator i, end;
	vector v;
	vector_create(&v, sizeof(e));

	ENSURE(vector_size(&v) == 0);

	for (e = 0; e < 10; ++e)
	{
		ENSURE(vector_push_back(&v, &e));
	}

	ENSURE(vector_size(&v) == 10);
	ENSURE(vector_capacity(&v) >= 10);

	for (i = vector_begin(&v), end = vector_end(&v); i != end; vector_next(&v, &i))
	{
		e = *(const element_t *)vector_deref(i);
		ENSURE(e == vector_distance(&v, vector_begin(&v), i));
	}

	vector_destroy(&v);
}

static void test_queue()
{
	typedef unsigned element_t;

	size_t i;
	element_t e;
	queue q;
	queue_create(&q, sizeof(e));

	for (e = 0, i = 0; i < 10; ++i)
	{
		ENSURE(queue_size(&q) == i / 2);
		ENSURE(queue_push(&q, &e));

		if (i % 2)
		{
			queue_pop(&q);
		}
	}

	for (i = 0; i < 5; ++i)
	{
		ENSURE(queue_size(&q) == 5 - i);
		queue_pop(&q);
		ENSURE(queue_size(&q) == 4 - i);
	}

	ENSURE(queue_empty(&q));
	ENSURE(queue_size(&q) == 0);

	queue_destroy(&q);
}

int main()
{
	test_hash_map();
	test_vector();
	return 0;
}
