#ifndef GAME_STATE_VIEW_H
#define GAME_STATE_VIEW_H


#include "SDL.h"


struct GameState;
struct GameStateView;


typedef struct GameStateViewType
{
	struct GameStateView *(*create)(struct GameState *);
	void (*destroy)(struct GameStateView *);
	void (*draw)(struct GameStateView *);
	void (*handle_event)(struct GameStateView *, SDL_Event const *);
}
GameStateViewType;


typedef struct GameStateView
{
	GameStateViewType const *type;
}
GameStateView;


#endif
