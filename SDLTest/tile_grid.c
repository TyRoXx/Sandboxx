#include "tile_grid.h"
#include <stdlib.h>


int TileGrid_init(TileGrid *g, size_t width, size_t height)
{
	g->tiles = calloc(width * height, sizeof(*g->tiles));
	g->width = width;
	g->height = height;
	return (g->tiles != 0);
}

void TileGrid_free(TileGrid *g)
{
	free(g->tiles);
}

TileIndex TileGrid_get(TileGrid const *g, size_t x, size_t y)
{
	return g->tiles[y * g->width + x];
}

void TileGrid_set(TileGrid const *g, size_t x, size_t y, TileIndex tile)
{
	g->tiles[y * g->width + x] = tile;
}
