#include "tile_grid.h"
#include <stdlib.h>
#include <assert.h>


Bool LayeredTile_is_walkable(LayeredTile const *tile)
{
	size_t i;
	for (i = 0; i < TILE_LAYER_COUNT; ++i)
	{
		TileKind const * const layer = tile->layers[i];

		/*cannot walk when there is no ground*/
		if (!layer && (i == 0))
		{
			return 0;
		}

		if (layer &&
			!layer->is_walkable)
		{
			return 0;
		}
	}
	return 1;
}


Bool TileGrid_init(TileGrid *g, size_t width, size_t height)
{
	assert(g);
	g->tiles = calloc(width * height, sizeof(*g->tiles));
	g->width = width;
	g->height = height;
	return (g->tiles != 0);
}

void TileGrid_free(TileGrid *g)
{
	assert(g);
	free(g->tiles);
}

LayeredTile *TileGrid_get(TileGrid const *g, size_t x, size_t y)
{
	assert(g);
	assert(x < g->width);
	assert(y < g->height);
	return &g->tiles[y * g->width + x];
}
