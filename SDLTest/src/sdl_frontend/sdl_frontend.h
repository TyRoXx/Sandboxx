#ifndef SDL_FRONTEND_H
#define SDL_FRONTEND_H


#include "base/frontend.h"
#include "image_manager.h"


struct Game;
struct GameStateView;


typedef struct SDLFrontend
{
	Frontend base;
	struct Game *game;
	SDL_Surface *screen;
	ImageManager images;
	struct GameStateView *state_view;
}
SDLFrontend;


Frontend *SDLFrontEnd_create(struct Game *game);


#endif
