#ifndef HASH_MAP_H
#define HASH_MAP_H


#include <stddef.h>


typedef size_t hash_t;

typedef struct hash_map_entry
{
	void *data;
}
hash_map_entry;

int hash_map_entry_create(hash_map_entry *entry,
	const void *key, size_t key_size,
	const void *value, size_t value_size);
void hash_map_entry_destroy(hash_map_entry *entry);

typedef struct hash_map_bucket
{
	hash_map_entry *entries;
	size_t count;
}
hash_map_bucket;

void hash_map_bucket_create(hash_map_bucket *bucket);
void hash_map_bucket_destroy(hash_map_bucket *bucket);
hash_map_entry *hash_map_bucket_get(hash_map_bucket *bucket, const void *key, size_t key_size);
int hash_map_bucket_add(
	hash_map_bucket *bucket,
	const void *key,
	size_t key_size,
	const void *value,
	size_t value_size);

typedef hash_t (*hash_function_t)(const void *, void *);

typedef struct hash_map
{
	hash_map_bucket *buckets;
	size_t key_size;
	size_t value_size;
	size_t elements;
	size_t bucket_count;
	hash_function_t hash;
	void *hash_user_data;
}
hash_map;

typedef struct hash_map_iterator
{
	hash_map *map;
	hash_map_bucket *bucket;
	hash_map_entry *entry;
}
hash_map_iterator;

hash_map_iterator hash_map_iterate(hash_map *map);
const void *hash_map_iterator_key(hash_map_iterator *iterator);
const void *hash_map_iterator_value(hash_map_iterator *iterator);
int hash_map_iterator_next(hash_map_iterator *iterator);
void hash_map_create(
	hash_map *map,
	size_t key_size,
	size_t value_size,
	hash_function_t hash,
	void *hash_user_data);
void hash_map_destroy(hash_map *map);
int hash_map_resize(hash_map *map, size_t bucket_count);
int hash_map_grow(hash_map *map);
hash_map_bucket *hash_map_find_bucket(const hash_map *map, const void *key);
int hash_map_insert(hash_map *map, const void *key, const void *value);
const void *hash_map_find(const hash_map *map, const void *key);
int hash_map_erase(hash_map *map, const void *key);


#endif
