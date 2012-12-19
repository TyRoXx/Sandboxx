#ifndef TILE_GRID_H
#define TILE_GRID_H


#include <stddef.h>


typedef struct TileKind
{
	size_t image_id;
	int is_walkable;
}
TileKind;

#define TILE_LAYER_COUNT 3

typedef struct LayeredTile
{
	TileKind const *layers[TILE_LAYER_COUNT];
}
LayeredTile;

typedef struct TileGrid
{
	LayeredTile *tiles; /*malloc'ed*/
	size_t width, height;
}
TileGrid;

int TileGrid_init(TileGrid *g, size_t width, size_t height);
void TileGrid_free(TileGrid *g);
LayeredTile *TileGrid_get(TileGrid const *g, size_t x, size_t y);


#endif
