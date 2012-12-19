#include "sdl_frontend.h"
#include "game.h"
#include "vector2f.h"
#include "SDL.h"
#include "SDL_main.h"
#include <stdlib.h>
#include <math.h>


enum
{
	Width = 640, Height = 480
};

typedef struct Camera
{
	Vector2f position;
}
Camera;

typedef struct Texture
{
	SDL_Surface *surface;
}
Texture;

typedef struct SDLFrontend
{
	Frontend base;
	Game *game;
	SDL_Surface *screen;
	Texture textures[5];
	Camera camera;
}
SDLFrontend;

static int load_bmp_texture(Texture *texture,
							char const *file_name,
							SDL_Color const *alpha_key)
{
	texture->surface = SDL_LoadBMP(file_name);
	if (!texture->surface)
	{
		fprintf(stderr, "Could not load image %s\n", file_name);
		return 0;
	}

	if (alpha_key)
	{
		SDL_SetColorKey(texture->surface,
						SDL_SRCCOLORKEY,
						SDL_MapRGB(texture->surface->format,
						alpha_key->r,
						alpha_key->g,
						alpha_key->b));
	}

	return 1;
}


static void SDLFrontend_destroy(Frontend *front)
{
	Texture *texture;
	SDLFrontend * const sdl_front = (SDLFrontend *)front;

	for (texture = sdl_front->textures;
		 texture != (sdl_front->textures +
				  sizeof(sdl_front->textures) / sizeof(sdl_front->textures[0]));
		 ++texture)
	{
		SDL_FreeSurface(texture->surface);
	}

	SDL_Quit();
}

static void draw_tiles(
	Camera const *camera,
	SDL_Surface *screen,
	TileGrid const *tiles,
	Texture const *textures,
	TileIndex texture_count)
{
	size_t const base_x = (size_t)camera->position.x - Width / 2;
	size_t const base_y = (size_t)camera->position.y - Height / 2;

	size_t ty;
	for (ty = 0; ty < tiles->height; ++ty)
	{
		size_t tx;
		for (tx = 0; tx < tiles->width; ++tx)
		{
			TileIndex const tile_index = TileGrid_get(tiles, tx, ty);
			SDL_Rect dest;
			SDL_Surface *image;

			if (tile_index >= texture_count)
			{
				continue;
			}

			image = textures[tile_index].surface;

			dest.x = (Sint16)((tx * (size_t)image->w) - base_x);
			dest.y = (Sint16)((ty * (size_t)image->h) - base_y);

			SDL_BlitSurface(image, 0, screen, &dest);
		}
	}
}

static void draw_background(SDL_Surface *screen)
{
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

			if (event.type == SDL_KEYUP)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_LEFT:
					sdl_front->camera.position.x -= 32;
					break;
				case SDLK_RIGHT:
					sdl_front->camera.position.x += 32;
					break;
				case SDLK_UP:
					sdl_front->camera.position.y -= 32;
					break;
				case SDLK_DOWN:
					sdl_front->camera.position.y += 32;
					break;
				default:
					break;
				}
			}
		}

		Game_update(sdl_front->game);

		draw_background(screen);
		draw_tiles(&sdl_front->camera,
				   screen,
				   &sdl_front->game->grid,
				   sdl_front->textures,
				   sizeof(sdl_front->textures) / sizeof(sdl_front->textures[0]));

		SDL_Flip(screen);

		SDL_Delay(10);
	}
}


static FrontendType const SDLFrontendType =
{
	SDLFrontend_destroy,
	SDLFrontend_main_loop
};

static SDL_Color const AlphaKey = {255, 0, 255, 0};
static char const * const WindowTitle = "SDL Test";


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

	SDL_WM_SetCaption(WindowTitle, WindowTitle);

	front->base.type = &SDLFrontendType;
	front->game = game;
	front->screen = SDL_SetVideoMode(Width, Height, 32, SDL_SWSURFACE);
	front->camera.position.x = Width / 2;
	front->camera.position.y = Height / 2;

	if (!front->screen)
	{
		fprintf(stderr, "Cannot create screen: %s\n", SDL_GetError());
		return 0;
	}

	if (
		!load_bmp_texture(front->textures + 0, "sprites/grass_32.bmp", 0) ||
		!load_bmp_texture(front->textures + 1, "sprites/dirt_32.bmp", 0) ||
		!load_bmp_texture(front->textures + 2, "sprites/n_grass_s_dirt_32.bmp", 0) ||
		!load_bmp_texture(front->textures + 3, "sprites/n_dirt_s_grass_32.bmp", 0) ||
		!load_bmp_texture(front->textures + 4, "sprites/fence_32.bmp", &AlphaKey))
	{
		return 0;
	}

	return (Frontend *)front;
}
