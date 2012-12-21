#include "world.h"
#include <stdlib.h>


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
