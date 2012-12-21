#ifndef WORLD_H
#define WORLD_H


#include "entity.h"


struct Map;

typedef struct World
{
	struct Map *current_map;
	struct EntityLink entities;
}
World;


int World_init(World *w, struct Map *map);
void World_free(World *w);
void World_update(World *w);


#endif
