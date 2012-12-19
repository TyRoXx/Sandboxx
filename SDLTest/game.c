#include "game.h"
#include <string.h>


enum
{
	MapWidth = 12, MapHeight = 8
};

static const TileIndex BuiltInMap[MapWidth * MapHeight] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 2, 2, 2, 2, 2, 5, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 0, 0,
	3, 3, 3, 3, 3, 3, 6, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};


int Game_init(Game *g)
{
	if (!TileGrid_init(&g->grid, 12, 8))
	{
		return 0;
	}

	memcpy(g->grid.tiles, BuiltInMap, sizeof(BuiltInMap));
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
