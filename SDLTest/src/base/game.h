#ifndef GAME_H
#define GAME_H


#include "world.h"
#include "game_state.h"


typedef struct Game
{
	GameState *state;
}
Game;


int Game_init(Game *g);
void Game_free(Game *g);
int Game_update(Game *g, unsigned delta);
int Game_enter_state(Game *g, GameStateDefinition const *state);


#endif
