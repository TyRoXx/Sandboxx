#ifndef WORLD_H
#define WORLD_H


#include "entity.h"
#include "tile_grid.h"


typedef struct World
{
	TileGrid tiles;
	Entity *entities;
	size_t entity_count;
}
World;


int World_init(World *w, TileGrid const *tiles);
void World_free(World *w);
void World_update(World *w, unsigned delta);
int World_add_entity(World *w, Entity const *entity);
int World_is_walkable(
	World const *world,
	ptrdiff_t x,
	ptrdiff_t y);


#endif
