#ifndef TILE_GRID_H
#define TILE_GRID_H


#include <stddef.h>


typedef size_t TileIndex;


typedef struct TileGrid
{
	TileIndex *tiles; /*malloc'ed*/
	size_t width, height;
}
TileGrid;

int TileGrid_init(TileGrid *g, size_t width, size_t height);
void TileGrid_free(TileGrid *g);
TileIndex TileGrid_get(TileGrid const *g, size_t x, size_t y);
void TileGrid_set(TileGrid const *g, size_t x, size_t y, TileIndex tile);


#endif
