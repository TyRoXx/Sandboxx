#include "world.h"
#include "map.h"
#include <stdlib.h>
#include <assert.h>


int World_init(World *w, struct Map *map)
{
	w->current_map = map;
	w->entities = 0;
	w->entity_count = 0;
	return 1;
}

void World_free(World *w)
{
	size_t i;
	for (i = 0; i < w->entity_count; ++i)
	{
		Entity_free(w->entities + i);
	}
	free(w->entities);
}

void World_update(World *w, unsigned delta)
{
	size_t i;
	for (i = 0; i < w->entity_count; ++i)
	{
		Entity_update(w->entities + i, delta);
	}
}

int World_add_entity(World *w, Entity const *entity)
{
	size_t const new_count = w->entity_count + 1;
	Entity * const new_entities = realloc(w->entities,
		new_count * sizeof(*new_entities));
	if (!new_entities)
	{
		return 0;
	}
	new_entities[new_count - 1] = *entity;
	w->entities = new_entities;
	w->entity_count = new_count;
	return 1;
}

static int is_entity_on(
	World const *world,
	ptrdiff_t x,
	ptrdiff_t y)
{
	size_t i;
	assert(world);

	for (i = 0; i < world->entity_count; ++i)
	{
		Entity const * const e = world->entities + i;
		assert(e);

		if (e->x == x &&
			e->y == y)
		{
			return 1;
		}
	}

	return 0;
}

static int is_walkable_tile(
	TileGrid const *tiles,
	ptrdiff_t x,
	ptrdiff_t y)
{
	if (x < 0 ||
		y < 0 ||
		x >= (ptrdiff_t)tiles->width ||
		y >= (ptrdiff_t)tiles->height)
	{
		return 0;
	}

	return LayeredTile_is_walkable(
		TileGrid_get(tiles, (size_t)x, (size_t)y)
		);
}

int World_is_walkable(
	World const *world,
	ptrdiff_t x,
	ptrdiff_t y)
{
	return
		is_walkable_tile(&world->current_map->terrain, x, y) &&
		!is_entity_on(world, x, y);
}
