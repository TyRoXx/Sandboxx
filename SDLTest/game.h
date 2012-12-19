#ifndef GAME_H
#define GAME_H


#include "tile_grid.h"


typedef struct Game
{
	TileGrid grid;
}
Game;


int Game_init(Game *g);
void Game_free(Game *g);
int Game_update();


#endif
