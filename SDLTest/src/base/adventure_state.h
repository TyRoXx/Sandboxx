#ifndef ADVENTURE_STATE_H
#define ADVENTURE_STATE_H


#include "game_state.h"
#include "world.h"


typedef struct AdventureState
{
	GameState base;
	World world;
	Entity *avatar;
}
AdventureState;


extern GameStateDefinition const AdventureStateDef;


#endif
