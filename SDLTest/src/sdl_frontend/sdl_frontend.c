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

static SDL_Color const AlphaKey = {255, 0, 255, 0};

static SDL_Surface *load_bmp_texture(char const *file_name,
									 SDL_PixelFormat *format)
{
	SDL_Surface * const bitmap = SDL_LoadBMP(file_name);
	SDL_Surface * converted;

	assert(format);

	if (!bitmap)
	{
		fprintf(stderr, "Could not load image %s\n", file_name);
		return 0;
	}

	converted = SDL_ConvertSurface(bitmap, format, 0);
	if (!converted)
	{
		fprintf(stderr, "Could not convert image %s\n", file_name);
		return bitmap;
	}

	SDL_FreeSurface(bitmap);

	if (SDL_SetColorKey(
			converted,
			SDL_SRCCOLORKEY,
			SDL_MapRGB(converted->format, AlphaKey.r, AlphaKey.g, AlphaKey.b)) < 0)
	{
		SDL_FreeSurface(converted);
		return 0;
	}

	return converted;
}


static void SDLFrontend_destroy(Frontend *front)
{
	SDLFrontend * const sdl_front = (SDLFrontend *)front;
	assert(sdl_front);

	if (sdl_front->state_view)
	{
		sdl_front->state_view->type->destroy(sdl_front->state_view);
	}

	ImageManager_free(&sdl_front->images);
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
static char const * const ImageFileNames[] =
{
	"data/sprites/grass_32.bmp",
	"data/sprites/dirt_32.bmp",
	"data/sprites/n_grass_s_dirt_32.bmp",
	"data/sprites/n_dirt_s_grass_32.bmp",
	"data/sprites/fence_32.bmp",
	"data/sprites/nw_dirt_on_grass_32.bmp",
	"data/sprites/w_dirt_e_grass_32.bmp",
	"data/sprites/sw_dirt_on_grass_32.bmp",
};

static int init_image_manager(ImageManager *images, SDL_PixelFormat *format)
{
	size_t const image_count = sizeof(ImageFileNames) / sizeof(*ImageFileNames);
	size_t i;

	assert(images);
	assert(format);

	if (!ImageManager_init(images, image_count))
	{
		return 0;
	}

	for (i = 0; i < image_count; ++i)
	{
		SDL_Surface * const image = load_bmp_texture(
					ImageFileNames[i],
					format
					);
		if (!image)
		{
			return 0;
		}
		PtrVector_set(&images->images, i, image);
	}

	return 1;
}

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

	if (!init_image_manager(&front->images, front->screen->format))
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
