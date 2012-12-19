#ifndef GAME_H
#define GAME_H


#include "map.h"


typedef struct Game
{
	Map current_map;
}
Game;


int Game_init(Game *g);
void Game_free(Game *g);
int Game_update(Game *g);


#endif
