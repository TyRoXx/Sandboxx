#ifndef GAME_H
#define GAME_H


#include "world.h"
#include "game_state.h"


typedef struct GameStateEnterCallback
{
	void (*function)(void *, GameState *);
	void *user_data;
}
GameStateEnterCallback;


typedef struct Game
{
	GameState *state;
	GameStateEnterCallback on_enter_state;
}
Game;


Bool Game_init(Game *g);
void Game_free(Game *g);
int Game_update(Game *g, unsigned delta);
int Game_enter_state(Game *g, GameStateDefinition const *state);


#endif
