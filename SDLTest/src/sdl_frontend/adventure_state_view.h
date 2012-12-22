#ifndef ADVENTURE_STATE_VIEW_H
#define ADVENTURE_STATE_VIEW_H


#include "game_state_view.h"


struct AdventureState;


typedef struct AdventureStateView
{
	GameStateView base;
	struct AdventureState *state;
}
AdventureStateView;


extern GameStateViewType const AdventureStateViewType;


#endif
