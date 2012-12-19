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
	2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
	3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0,
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
};

static void init_demo_grid(TileGrid *g)
{
	size_t y;
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
}


int Game_init(Game *g)
{
	if (!TileGrid_init(&g->grid, 12, 8))
	{
		return 0;
	}

	init_demo_grid(&g->grid);
	return 1;
}

void Game_free(Game *g)
{
	TileGrid_free(&g->grid);
}

int Game_update(Game *g)
{
	return 1;
}
