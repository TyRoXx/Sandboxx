#include "tile_grid.h"
#include <stdlib.h>
#include <assert.h>


int TileGrid_init(TileGrid *g, size_t width, size_t height)
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
