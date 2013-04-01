#ifndef WORLD_H
#define WORLD_H


#include "entity.h"
#include "tile_grid.h"
#include "bool.h"


typedef struct World
{
	TileGrid tiles;
	Entity *entities;
	size_t entity_count;
}
World;


Bool World_init(World *w, TileGrid const *tiles);
void World_free(World *w);
void World_update(World *w, unsigned delta);
Bool World_add_entity(World *w, Entity const *entity);
Bool World_is_walkable(
	World const *world,
	Vector2i const *position);


#endif
