#ifndef MAP_H
#define MAP_H


#include "tile_grid.h"


typedef struct Map
{
	TileGrid terrain;
}
Map;


void Map_free(Map *map);


#endif
