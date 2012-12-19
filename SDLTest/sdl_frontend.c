#include "sdl_frontend.h"
#include "game.h"
#include "SDL.h"
#include "SDL_main.h"
#include <stdlib.h>
#include <math.h>


enum
{
	Width = 640, Height = 480
};

typedef struct SDLFrontend
{
	Frontend base;
	Game *game;
	SDL_Surface *screen;
	SDL_Surface *tile_images[4];
}
SDLFrontend;


static void SDLFrontend_destroy(Frontend *front)
{
	SDL_Surface **tile;
	SDLFrontend * const sdl_front = (SDLFrontend *)front;

	for (tile = sdl_front->tile_images;
		 tile != (sdl_front->tile_images +
				  sizeof(sdl_front->tile_images) / sizeof(sdl_front->tile_images[0]));
		 ++tile)
	{
		SDL_FreeSurface(*tile);
	}

	SDL_Quit();
}

static void draw_tiles(
	SDL_Surface *screen,
	size_t x,
	size_t y,
	TileGrid const *tiles,
	SDL_Surface * const *tile_images,
	TileIndex tile_image_count)
{
	size_t ty;
	for (ty = 0; ty < tiles->height; ++ty)
	{
		size_t tx;
		for (tx = 0; tx < tiles->width; ++tx)
		{
			TileIndex const tile_index = TileGrid_get(tiles, tx, ty);
			SDL_Rect dest;
			SDL_Surface *image;

			if (tile_index >= tile_image_count)
			{
				continue;
			}

			image = tile_images[tile_index];

			dest.x = x + (tx * image->w);
			dest.y = y + (ty * image->h);

			SDL_BlitSurface(image, 0, screen, &dest);
		}
	}
}

static void SDLFrontend_main_loop(Frontend *front)
{
	SDLFrontend * const sdl_front = (SDLFrontend *)front;
	SDL_Surface * const screen = sdl_front->screen;
	int is_running = 1;

	while (is_running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				is_running = 0;
				break;
			}
		}

		Game_update(sdl_front->game);

		if (screen->format->BytesPerPixel == 4)
		{
			unsigned x, y;
			unsigned const time_ms = SDL_GetTicks();

			for (y = 0; y < Height; ++y)
			{
				for (x = 0; x < Width; ++x)
				{
					uint32_t const pixel = SDL_MapRGB(screen->format,
												(Uint8)((1 + sin((float)time_ms / 2800.0f)) * 128),
												(Uint8)((1 + cos((float)time_ms / 1700.0f)) * 128),
												(Uint8)((float)y / (float)Height * 256.0f));

					memcpy(((char *)screen->pixels) + ((Width * y) + x) * 4,
						   &pixel,
						   4);
				}
			}
		}

		draw_tiles(screen,
				   64,
				   64,
				   &sdl_front->game->grid,
				   sdl_front->tile_images,
				   sizeof(sdl_front->tile_images) / sizeof(sdl_front->tile_images[0]));

		SDL_UpdateRect(screen, 0, 0, Width, Height);

		SDL_Delay(10);
	}
}


static FrontendType const SDLFrontendType =
{
	SDLFrontend_destroy,
	SDLFrontend_main_loop
};


Frontend *SDLFrontEnd_create(struct Game *game)
{
	SDLFrontend * const front = malloc(sizeof(*front));
	if (!front)
	{
		return 0;
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		 fprintf(stderr, "SDL error: %s\n", SDL_GetError());
		 return 0;
	}

	front->base.type = &SDLFrontendType;
	front->game = game;
	front->screen = SDL_SetVideoMode(Width, Height, 32, SDL_SWSURFACE);

	if (!front->screen)
	{
		fprintf(stderr, "Cannot create screen: %s\n", SDL_GetError());
		return 0;
	}

	front->tile_images[0] = SDL_LoadBMP("sprites/grass_32.bmp");
	front->tile_images[1] = SDL_LoadBMP("sprites/dirt_32.bmp");
	front->tile_images[2] = SDL_LoadBMP("sprites/n_grass_s_dirt_32.bmp");
	front->tile_images[3] = SDL_LoadBMP("sprites/n_dirt_s_grass_32.bmp");

	if (!front->tile_images[0] ||
			!front->tile_images[1] ||
			!front->tile_images[2] ||
			!front->tile_images[3]
			)
	{
		return 0;
	}

	return (Frontend *)front;
}
