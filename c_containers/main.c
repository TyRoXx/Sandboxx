#include "hash_map.h"
#include "hash_set.h"
#include "queue.h"
#include "linked_list.h"
#include "vector.h"
#include "stack.h"
#include "tree_map.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>


#define ENSURE(x) { const int ensure_success_variable = !!(x); assert(ensure_success_variable); }

typedef long long map_key;
typedef char value_t;

static hash_t hash(const void *key, void *user_data)
{
	map_key original;
	memcpy(&original, key, sizeof(original));
	return (hash_t)(original * 1337);
}

static void print_map(hash_map *map)
{
	hash_map_iterator i = hash_map_iterate(map);
	while (hash_map_iterator_next(&i))
	{
		map_key key;
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
		map_key key;
		memcpy(&key, hash_set_iterator_key(&i), sizeof(key));
		printf("%d\n", (int)key);
	}
}

static void test_hash_map()
{
	size_t j;

	hash_map map;
	hash_map_create(&map, sizeof(map_key), sizeof(value_t), hash, 0);

	for (j = 0; j < 3; ++j)
	{
		map_key key = 0;
		value_t value = 'A';

		hash_map_clear(&map);

		for (; key < 26; ++key)
		{
			ENSURE(hash_map_insert(&map, &key, &value));
			ENSURE(hash_map_find_bucket(&map, &key));
			ENSURE(hash_map_find(&map, &key));

			++value;
		}

		for (key = 0; key < 26; key += 2)
		{
			ENSURE(hash_map_find_bucket(&map, &key));
			ENSURE(hash_map_find(&map, &key));
			hash_map_erase(&map, &key);
			ENSURE(!hash_map_find(&map, &key));
		}

		ENSURE(hash_map_size(&map) == 13);
	}

	hash_map_destroy(&map);
}

static void test_hash_set()
{
	size_t j;

	hash_set set;
	hash_set_create(&set, sizeof(map_key), hash, 0);

	for (j = 0; j < 3; ++j)
	{
		map_key key = 0;

		hash_set_clear(&set);

		for (; key < 26; ++key)
		{
			ENSURE(hash_set_insert(&set, &key));
			ENSURE(hash_set_contains(&set, &key));
		}

		for (key = 0; key < 26; key += 2)
		{
			ENSURE(hash_set_contains(&set, &key));
			hash_set_erase(&set, &key);
			ENSURE(!hash_set_contains(&set, &key));
		}

		ENSURE(hash_set_size(&set) == 13);
	}

	hash_set_destroy(&set);
}

static void test_vector()
{
	typedef unsigned element_t;

	size_t j;
	element_t e;
	vector_iterator i, end;
	vector v;
	vector_create(&v, sizeof(e));

	for (j = 0; j < 3; ++j)
	{
		vector_clear(&v);

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
	}

	vector_destroy(&v);
}

static void test_queue()
{
	typedef unsigned element_t;

	size_t j;
	size_t i;
	size_t size;
	element_t e;
	queue q;
	queue_create(&q, sizeof(e));

	for (j = 0; j < 3; ++j)
	{
		queue_clear(&q);

		for (e = 0, i = 0; i < 10; ++i)
		{
			size = queue_size(&q);
			ENSURE(size == (i + 1) / 2);
			ENSURE(queue_push(&q, &e));

			if (i % 2)
			{
				queue_pop(&q);
			}
		}

		for (i = 0; i < 5; ++i)
		{
			size = queue_size(&q);
			ENSURE(size == 5 - i);

			queue_pop(&q);

			size = queue_size(&q);
			ENSURE(size == 4 - i);
		}

		ENSURE(queue_empty(&q));
		ENSURE(queue_size(&q) == 0);
	}

	queue_destroy(&q);
}

static void test_stack()
{
	typedef long long element_t;

	size_t j;
	element_t e;
	element_t d;
	stack s;
	stack_create(&s, sizeof(e));

	for (j = 0; j < 3; ++j)
	{
		stack_clear(&s);

		ENSURE(stack_empty(&s));

		for (e = 0; e < 20; ++e)
		{
			ENSURE(stack_push(&s, &e));
		}

		ENSURE(!stack_empty(&s));

		while (!stack_empty(&s))
		{
			d = *(const element_t *)stack_top(&s);
			--e;
			ENSURE(d == e);
			stack_pop(&s);
		}

		ENSURE(stack_empty(&s));
	}

	ENSURE(stack_push(&s, &e));
	stack_destroy(&s);
}

static int compare_uchar(const void *left, const void *right, void *user)
{
	return memcmp(left, right, 1);
}

static void test_tree_map()
{
	typedef unsigned char map_key;
	typedef double value_t;

	size_t j;
	map_key key;
	value_t value;
	void *found;
	tree_map map;
	tree_map_create(&map, sizeof(key), sizeof(value), compare_uchar, 0);

	for (j = 0; j < 3; ++j)
	{
		tree_map_clear(&map);

		for (key = 'a'; key < 'z'; ++key)
		{
			value = key * -123;
			ENSURE(tree_map_insert(&map, &key, &value));
		}

		for (key = 'a'; key < 'z'; ++key)
		{
			value = key * -123;
			found = tree_map_find(&map, &key);
			ENSURE(found);
			ENSURE(!memcmp(found, &value, sizeof(value)));

#if 0
			tree_map_erase(&map, &key);
			ENSURE(!tree_map_find(&map, &key));
#endif
		}
	}

	tree_map_destroy(&map);
}

int main()
{
	size_t i;

	for (i = 0; i < 4; ++i)
	{
		test_hash_map();
		test_hash_set();
		test_vector();
		test_queue();
		test_stack();
		test_tree_map();
	}

	return 0;
}
