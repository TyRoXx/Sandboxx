#ifndef WORLD_H
#define WORLD_H


#include "entity.h"
#include "tile_grid.h"
#include "bool.h"
#include "vector.h"


typedef struct World
{
	TileGrid tiles;
	Vector movers;
}
World;

void World_free(World *w);
void World_update(World *w, unsigned delta);
Bool World_add_entity(World *w, Entity const *entity);
Bool World_add_mover(World *w, Mover mover);
Bool World_is_walkable(
	World const *world,
	Vector2i const *position);

void free_movers(Vector *movers);


#endif
