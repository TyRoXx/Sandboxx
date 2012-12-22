#ifndef GAME_H
#define GAME_H


#include "world.h"


typedef struct Game
{
	World world;
	Entity *avatar;
}
Game;


int Game_init(Game *g);
void Game_free(Game *g);
int Game_update(Game *g, unsigned delta);


#endif
