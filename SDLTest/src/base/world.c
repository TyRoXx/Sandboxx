#include "world.h"
#include <stdlib.h>
#include <assert.h>


void World_free(World *w)
{
	size_t i;
	for (i = 0; i < w->entity_count; ++i)
	{
		Entity_free(w->entities + i);
	}
	free(w->entities);
	TileGrid_free(&w->tiles);
}

void World_update(World *w, unsigned delta)
{
	size_t i;
	for (i = 0; i < w->entity_count; ++i)
	{
		Entity_update(w->entities + i, delta);
	}
}

Bool World_add_entity(World *w, Entity const *entity)
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

static Bool is_entity_on(
	World const *world,
	Vector2i const *position)
{
	size_t i;
	assert(world);

	for (i = 0; i < world->entity_count; ++i)
	{
		Entity const * const e = world->entities + i;
		assert(e);

		if (Vector2i_equal(position, &e->position))
		{
			return 1;
		}
	}

	return 0;
}

static Bool is_walkable_tile(
	TileGrid const *tiles,
	Vector2i const *position)
{
	if (position->x < 0 ||
		position->y < 0 ||
		position->x >= (ptrdiff_t)tiles->width ||
		position->y >= (ptrdiff_t)tiles->height)
	{
		return 0;
	}

	return LayeredTile_is_walkable(
		TileGrid_get(tiles, (size_t)position->x, (size_t)position->y)
		);
}

Bool World_is_walkable(
	World const *world,
	Vector2i const *position)
{
	return
		is_walkable_tile(&world->tiles, position) &&
		!is_entity_on(world, position);
}
