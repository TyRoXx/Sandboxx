#include "game.h"
#include "map_text_file.h"
#include <string.h>
#include <stdio.h>


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
	{4, 0},
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

	a.tile_set_id = 4;

	if (!Entity_init(&e, 3, 4, a, 1.7f, w))
	{
		return 0;
	}

	e.direction = Dir_North;

	if (!World_add_entity(w, &e))
	{
		Entity_free(&e);
		return 0;
	}

	return 1;
}

static void save_world(World const *world)
{
	FILE * const file = fopen("world.txt", "w");
	if (!file)
	{
		return;
	}
	save_world_to_text(world, TileKinds, file);
}

int Game_init(Game *g)
{
	TileGrid demo_tiles;

	if (!TileGrid_init(&demo_tiles, 12, 8))
	{
		return 0;
	}

	init_demo_grid(&demo_tiles);

	if (!World_init(&g->world, &demo_tiles) ||
		!add_demo_entities(&g->world))
	{
		goto fail_0;
	}

	save_world(&g->world);

	/*if there is an entity, choose the first one as the avatar*/
	g->avatar = g->world.entities;
	return 1;

fail_0:
	TileGrid_free(&demo_tiles);
	return 0;
}

void Game_free(Game *g)
{
	World_free(&g->world);
}

int Game_update(Game *g, unsigned delta)
{
	World_update(&g->world, delta);
	return 1;
}
