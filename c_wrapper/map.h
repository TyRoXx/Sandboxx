#ifndef MAP_H
#define MAP_H


#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

struct map_t;
typedef struct map_t map_t;


map_t *map_create();
int map_insert(map_t *map, const char *key, const char *value);
void map_erase(map_t *map, const char *key);
size_t map_size(map_t *map);
const char *map_find(map_t *map, const char *key);
void map_destroy(map_t *map);


#ifdef __cplusplus
};
#endif


#endif
