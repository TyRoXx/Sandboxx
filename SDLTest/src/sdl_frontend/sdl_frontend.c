#include "sdl_frontend.h"
#include "adventure_state_view.h"
#include "base/adventure_state.h"
#include "base/game.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_main.h"
#include <assert.h>

enum
{
	Width = 640, Height = 480
};

static void SDLFrontend_destroy(Frontend *front)
{
	SDLFrontend * const sdl_front = (SDLFrontend *)front;
	assert(sdl_front);

	if (sdl_front->state_view)
	{
		sdl_front->state_view->type->destroy(sdl_front->state_view);
	}

	Data_free(&sdl_front->data);
	free(front);

	TTF_Quit();
	SDL_Quit();
}

static void SDLFrontend_main_loop(Frontend *front)
{
	SDLFrontend * const sdl_front = (SDLFrontend *)front;
	SDL_Surface * const screen = sdl_front->screen;
	Game * const game = sdl_front->game;
	int is_running = 1;
	unsigned last_time = SDL_GetTicks();

	assert(sdl_front->state_view);

	while (is_running)
	{
		{
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT)
				{
					is_running = 0;
					break;
				}

				if (event.type == SDL_KEYUP)
				{
					switch (event.key.keysym.sym)
					{
					case SDLK_ESCAPE:
						is_running = 0;
						break;
					default:
						break;
					}
				}

				sdl_front->state_view->type->handle_event(
							sdl_front->state_view,
							&event);
			}
		}

		{
			unsigned current_time;
			current_time = SDL_GetTicks();
			assert(current_time >= last_time);
			Game_update(game, (current_time - last_time));
			last_time = current_time;
		}

		sdl_front->state_view->type->update(sdl_front->state_view);

		SDL_FillRect(screen, 0, 0);

		sdl_front->state_view->type->draw(sdl_front->state_view);

		SDL_Flip(screen);
		SDL_Delay(16);
	}
}


static FrontendType const SDLFrontendType =
{
	SDLFrontend_destroy,
	SDLFrontend_main_loop
};

static char const * const WindowTitle = "SDL Test";

static void on_enter_game_state(void *user_data, GameState *state)
{
	SDLFrontend * const sdl_front = user_data;

	assert(sdl_front);
	assert(state);
	assert(state->definition == &AdventureStateDef);

	sdl_front->state_view = AdventureStateViewType.create(state, sdl_front);
	if (!sdl_front->state_view)
	{
		/*TODO*/
		return;
	}
	sdl_front->state_view->type = &AdventureStateViewType;
}

Frontend *SDLFrontEnd_create(struct Game *game)
{
	SDLFrontend * const front = malloc(sizeof(*front));
	if (!front)
	{
		goto fail_0;
	}

	assert(game);

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		 fprintf(stderr, "SDL error: %s\n", SDL_GetError());
		 goto fail_1;
	}

	if (TTF_Init() == -1)
	{
		fprintf(stderr, "SDL_tff error: %s\n", TTF_GetError());
		goto fail_2;
	}

	front->base.type = &SDLFrontendType;
	front->game = game;
	front->screen = SDL_SetVideoMode(Width, Height, 32,
									 SDL_HWSURFACE | SDL_DOUBLEBUF);
	front->state_view = 0;

	if (!front->screen)
	{
		fprintf(stderr, "Cannot create screen: %s\n", SDL_GetError());
		goto fail_3;
	}

	if (!Data_init(&front->data, "data", front->screen->format))
	{
		goto fail_3;
	}

	assert(!game->on_enter_state.function);
	game->on_enter_state.function = on_enter_game_state;
	game->on_enter_state.user_data = front;

	SDL_WM_SetCaption(WindowTitle, WindowTitle);

	return (Frontend *)front;

fail_3:
	TTF_Quit();

fail_2:
	SDL_Quit();

fail_1:
	free(front);

fail_0:
	return 0;
}
