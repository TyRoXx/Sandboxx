#include "map.h"


void Map_free(Map *map)
{
	TileGrid_free(&map->terrain);
}
