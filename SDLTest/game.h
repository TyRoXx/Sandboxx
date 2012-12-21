#ifndef GAME_H
#define GAME_H


#include "world.h"
#include "map.h"


typedef struct Game
{
	World world;
	Map current_map;
}
Game;


int Game_init(Game *g);
void Game_free(Game *g);
int Game_update(Game *g);


#endif
