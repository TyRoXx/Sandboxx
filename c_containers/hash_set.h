#ifndef HASH_SET_H
#define HASH_SET_H


#include "hash_map.h"


typedef struct hash_set
{
	hash_map map;
}
hash_set;

typedef hash_map_iterator hash_set_iterator;


hash_set_iterator hash_set_iterate(hash_set *set);
const void *hash_set_iterator_key(hash_set_iterator *iterator);
int hash_set_iterator_next(hash_set_iterator *iterator);
void hash_set_create(
	hash_set *set,
	size_t key_size,
	hash_function_t hash,
	void *hash_user_data);
void hash_set_destroy(hash_set *set);
int hash_set_resize(hash_set *set, size_t bucket_count);
int hash_set_grow(hash_set *set);
int hash_set_insert(hash_set *set, const void *key);
int hash_set_contains(const hash_set *set, const void *key);
int hash_set_erase(hash_set *set, const void *key);
size_t hash_set_size(hash_set *set);
void hash_set_clear(hash_set *set);


#endif
