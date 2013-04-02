#include "world.h"
#include "algorithm.h"
#include <stdlib.h>
#include <assert.h>


void World_free(World *w)
{
	free_movers(&w->movers);
	TileGrid_free(&w->tiles);
}

static void update_mover(void *element, void *user)
{
	unsigned const delta = *(unsigned *)user;
	(void)user;
	Mover_update(element, delta);
}

void World_update(World *w, unsigned delta)
{
	for_each(Vector_begin(&w->movers),
			 Vector_end(&w->movers),
			 sizeof(Mover),
			 update_mover,
			 &delta);
}

Bool World_add_mover(World *w, Mover mover)
{
	return Vector_push_back(&w->movers, &mover, sizeof(mover));
}

static Bool is_mover_on_position(void *element, void *user)
{
	Mover const * const mover = element;
	Vector2i const * const position = user;
	return Vector2i_equal(position, &mover->body.position);
}

static Bool is_entity_on(
	World const *world,
	Vector2i const *position)
{
	void *match;
	void * const end = Vector_end(&world->movers);

	assert(world);

	match = find(Vector_begin(&world->movers),
				 end,
				 sizeof(Mover),
				 is_mover_on_position,
				 (void *)position);
	return (match != end);
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


static void free_mover(void *element, void *user)
{
	(void)user;
	Mover_free(element);
}

void free_movers(Vector *movers)
{
	for_each(Vector_begin(movers),
			 Vector_end(movers),
			 sizeof(Mover),
			 free_mover,
			 NULL);
	Vector_free(movers);
}
