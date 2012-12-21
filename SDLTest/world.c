#include "world.h"


int World_init(World *w, struct Map *map)
{
	w->current_map = map;
	w->entities.next = w->entities.previous = 0;
	return 1;
}

void World_free(World *w)
{
	(void)w;
}

void World_update(World *w)
{
	(void)w;
}
