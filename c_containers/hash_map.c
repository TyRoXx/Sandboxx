#include "hash_map.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>


struct hash_map_entry
{
	void *data;
};

struct hash_map_bucket
{
	hash_map_entry *entries;
	size_t count;
};

static int hash_map_entry_create(
	hash_map_entry *entry,
	const void *key, size_t key_size,
	const void *value, size_t value_size)
{
	entry->data = malloc(key_size + value_size);
	if (!entry->data)
	{
		return 0;
	}
	memcpy(entry->data, key, key_size);
	memcpy(((char *)entry->data) + key_size, value, value_size);
	return 1;
}

static void hash_map_entry_destroy(hash_map_entry *entry)
{
	free(entry->data);
}

static void hash_map_bucket_create(hash_map_bucket *bucket)
{
	bucket->entries = 0;
	bucket->count = 0;
}

static void hash_map_bucket_destroy(hash_map_bucket *bucket)
{
	size_t i;
	for (i = 0; i < bucket->count; ++i)
	{
		hash_map_entry *entry = bucket->entries + i;
		hash_map_entry_destroy(entry);
	}
	free(bucket->entries);
}

static hash_map_entry *hash_map_bucket_get(hash_map_bucket *bucket, const void *key, size_t key_size)
{
	size_t i;
	for (i = 0; i < bucket->count; ++i)
	{
		hash_map_entry *entry = bucket->entries + i;
		if (!memcmp(entry->data, key, key_size))
		{
			return entry;
		}
	}
	return 0;
}

static int hash_map_bucket_add(
	hash_map_bucket *bucket,
	const void *key,
	size_t key_size,
	const void *value,
	size_t value_size)
{
	hash_map_entry *entries = realloc(bucket->entries, sizeof(*entries) * (bucket->count + 1));
	if (!entries)
	{
		return 0;
	}
	if (!hash_map_entry_create(
		entries + bucket->count,
		key,
		key_size,
		value,
		value_size))
	{
		return 0;
	}
	bucket->entries = entries;
	++(bucket->count);
	return 1;
}

static void hash_map_bucket_clear(
	hash_map_bucket *bucket)
{
	hash_map_bucket_destroy(bucket);
	bucket->entries = 0;
	bucket->count = 0;
}


hash_map_iterator hash_map_iterate(const hash_map *map)
{
	hash_map_iterator iterator;
	iterator.map = map;
	iterator.bucket = map->buckets;
	iterator.entry = 0;
	return iterator;
}

const void *hash_map_iterator_key(const hash_map_iterator *iterator)
{
	return iterator->entry->data;
}

const void *hash_map_iterator_value(const hash_map_iterator *iterator)
{
	return ((const char *)iterator->entry->data) + iterator->map->key_size;
}

int hash_map_iterator_next(hash_map_iterator *iterator)
{
	if (iterator->entry)
	{
		++(iterator->entry);
		while (iterator->entry == iterator->bucket->entries + iterator->bucket->count)
		{
			++(iterator->bucket);
			if (iterator->bucket == iterator->map->buckets + iterator->map->bucket_count)
			{
				return 0;
			}
			iterator->entry = iterator->bucket->entries;
		}
		return 1;
	}
	else
	{
		do 
		{
			if (iterator->bucket == iterator->map->buckets + iterator->map->bucket_count)
			{
				return 0;
			}
			iterator->entry = iterator->bucket->entries;
			if (!iterator->entry)
			{
				++(iterator->bucket);
			}
		} while (!iterator->entry);
		return 1;
	}
}

void hash_map_create(
	hash_map *map,
	size_t key_size,
	size_t value_size,
	hash_function_t hash,
	void *hash_user_data)
{
	map->buckets = 0;
	map->key_size = key_size;
	map->value_size = value_size;
	map->elements = 0;
	map->bucket_count = 0;
	map->hash = hash;
	map->hash_user_data = hash_user_data;
}

void hash_map_destroy(hash_map *map)
{
	size_t i;
	for (i = 0; i < map->bucket_count; ++i)
	{
		hash_map_bucket_destroy(map->buckets + i);
	}
	free(map->buckets);
}

int hash_map_resize(hash_map *map, size_t bucket_count)
{
	hash_map resized;
	size_t i;
	hash_map_iterator iterator;

	assert(map);
	assert(bucket_count > 0);

	hash_map_create(&resized, map->key_size, map->value_size, map->hash, map->hash_user_data);
	resized.buckets = malloc(sizeof(*resized.buckets) * bucket_count);
	if (!resized.buckets)
	{
		hash_map_destroy(&resized);
		return 0;
	}
	for (i = 0; i < bucket_count; ++i)
	{
		hash_map_bucket_create(resized.buckets + i);
	}
	resized.bucket_count = bucket_count;

	iterator = hash_map_iterate(map);

	while (hash_map_iterator_next(&iterator))
	{
		const void *key = hash_map_iterator_key(&iterator);
		const void *value = hash_map_iterator_value(&iterator);
		if (!hash_map_insert(&resized, key, value))
		{
			hash_map_destroy(&resized);
			return 0;
		}
	}

	hash_map_destroy(map);
	*map = resized;
	return 1;
}

int hash_map_grow(hash_map *map)
{
	if (map->elements >= map->bucket_count)
	{
		size_t new_size = map->elements * 2;
		assert(!new_size || (new_size > map->elements));

		if (new_size < 4)
		{
			new_size = 4; 
		}
		return hash_map_resize(map, new_size);
	}
	return 1;
}

hash_map_bucket *hash_map_find_bucket(const hash_map *map, const void *key)
{
	if (!map->bucket_count)
	{
		return 0;
	}
	else
	{
		const hash_t code = map->hash(key, map->hash_user_data);
		hash_map_bucket *bucket = map->buckets + (code % map->bucket_count);
		return bucket;
	}
}

int hash_map_insert(hash_map *map, const void *key, const void *value)
{
	hash_map_bucket *bucket;
	if (!hash_map_grow(map))
	{
		return 0;
	}
	bucket = hash_map_find_bucket(map, key);
	if (hash_map_bucket_get(bucket, key, map->key_size))
	{
		return 1;
	}
	else
	{
		if (hash_map_bucket_add(bucket, key, map->key_size, value, map->value_size))
		{
			++(map->elements);
			return 1;
		}
		return 0;
	}
}

const void *hash_map_find(const hash_map *map, const void *key)
{
	hash_map_bucket *bucket = hash_map_find_bucket(map, key);
	hash_map_entry *entry = hash_map_bucket_get(bucket, key, map->key_size);
	return ((char *)entry->data) + map->key_size;
}

int hash_map_erase(hash_map *map, const void *key)
{
	hash_map_bucket *bucket = hash_map_find_bucket(map, key);
	hash_map_entry *entry;
	hash_map_entry entry_copy;
	hash_map_entry *back;
	hash_map_entry *new_entries;
	if (!bucket)
	{
		return 0;
	}
	entry = hash_map_bucket_get(bucket, key, map->key_size);
	if (!entry)
	{
		return 0;
	}
	entry_copy = *entry;
	back = bucket->entries + (bucket->count - 1);
	if (entry != back)
	{
		*entry = *back;
	}
	new_entries = realloc(bucket->entries, sizeof(*new_entries) * (bucket->count - 1));
	if (!new_entries &&
		(bucket->count > 1))
	{
		*back = entry_copy;
		return 0;
	}
	bucket->entries = new_entries;
	--(bucket->count);
	assert(map->elements);
	--(map->elements);
	return 1;
}

size_t hash_map_size(const hash_map *map)
{
	return map->elements;
}

void hash_map_clear(hash_map *map)
{
	size_t b;
	for (b = 0; b < map->bucket_count; ++b)
	{
		hash_map_bucket *bucket = map->buckets + b;
		hash_map_bucket_clear(bucket);
	}
	map->elements = 0;
}
