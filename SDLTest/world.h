#ifndef WORLD_H
#define WORLD_H


#include "entity.h"


struct Map;

typedef struct World
{
	struct Map *current_map;
	Entity *entities;
	size_t entity_count;
}
World;


int World_init(World *w, struct Map *map);
void World_free(World *w);
void World_update(World *w, unsigned delta);
int World_add_entity(World *w, Entity const *entity);


#endif
