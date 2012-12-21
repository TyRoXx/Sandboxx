#include "game.h"
#include <string.h>


enum
{
	MapWidth = 12, MapHeight = 8
};

static size_t const BuiltInMap[MapWidth * MapHeight] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 2, 2, 2, 2, 2, 7, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 6, 0, 0, 0, 0, 0,
	3, 3, 3, 3, 3, 3, 5, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static TileKind const TileKinds[] =
{
	{0, 1},
	{1, 1},
	{2, 1},
	{3, 1},
	{4, 1},
	{5, 1},
	{6, 1},
	{7, 1},
};

static void init_demo_grid(TileGrid *g)
{
	size_t y, i;

	for (y = 0; y < MapHeight; ++y)
	{
		size_t x;
		for (x = 0; x < MapWidth; ++x)
		{
			size_t index = y * MapWidth + x;
			g->tiles[index].layers[0] =
					&TileKinds[BuiltInMap[index]];
		}
	}

	for (i = 2; i < MapWidth - 2; ++i)
	{
		g->tiles[5 * MapWidth + i].layers[1] = &TileKinds[4];
	}
}

static int add_demo_entities(World *w)
{
	Appearance a;
	Entity e;

	a.radius = 1;
	a.tile_set_id = 4;

	if (!Entity_init(&e, 3, 4, a, 1.0f))
	{
		return 0;
	}

	e.is_moving = 1;
	e.move_progress = 0;
	e.direction = Dir_North;

	if (!World_add_entity(w, &e))
	{
		Entity_free(&e);
		return 0;
	}

	return 1;
}

int Game_init(Game *g)
{
	if (!TileGrid_init(&g->current_map.terrain, 12, 8))
	{
		return 0;
	}

	init_demo_grid(&g->current_map.terrain);

	if (!World_init(&g->world, &g->current_map) ||
		!add_demo_entities(&g->world))
	{
		goto fail_0;
	}

	g->avatar = g->world.entities;
	return 1;

fail_0:
	Map_free(&g->current_map);
	return 0;
}

void Game_free(Game *g)
{
	World_free(&g->world);
	Map_free(&g->current_map);
}

int Game_update(Game *g, unsigned delta)
{
	World_update(&g->world, delta);
	return 1;
}
