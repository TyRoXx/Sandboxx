#include "sdl_frontend.h"
#include "game.h"
#include "vector2f.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_main.h"
#include <stdlib.h>
#include <math.h>
#include <assert.h>


enum
{
	Width = 640, Height = 480
};

typedef struct Camera
{
	Vector2f position;
}
Camera;

typedef struct ImageManager
{
	SDL_Surface **images;
	size_t image_count;
}
ImageManager;

static int ImageManager_init(ImageManager *im, size_t image_count)
{
	im->images = calloc(image_count, sizeof(*im->images));
	if (im->images)
	{
		im->image_count = image_count;
		return 1;
	}
	return 0;
}

static void ImageManager_free(ImageManager *im)
{
	size_t i;
	for (i = 0; i < im->image_count; ++i)
	{
		SDL_Surface * const s = im->images[i];
		SDL_FreeSurface(s);
	}
	free(im->images);
}

typedef struct SDLFrontend
{
	Frontend base;
	Game *game;
	SDL_Surface *screen;
	ImageManager images;
	Camera camera;
}
SDLFrontend;

static SDL_Color const AlphaKey = {255, 0, 255, 0};

static SDL_Surface *load_bmp_texture(
							char const *file_name)
{
	SDL_Surface * const surface = SDL_LoadBMP(file_name);
	if (!surface)
	{
		fprintf(stderr, "Could not load image %s\n", file_name);
		return 0;
	}

	SDL_SetColorKey(surface,
					SDL_SRCCOLORKEY,
					SDL_MapRGB(surface->format,
					AlphaKey.r,
					AlphaKey.g,
					AlphaKey.b));

	return surface;
}


static void SDLFrontend_destroy(Frontend *front)
{
	SDLFrontend * const sdl_front = (SDLFrontend *)front;

	ImageManager_free(&sdl_front->images);

	TTF_Quit();
	SDL_Quit();
}

static void draw_layered_tile(
		size_t x,
		size_t y,
		SDL_Surface *screen,
		LayeredTile const *tile,
		ImageManager const *images
		)
{
	size_t i;
	SDL_Rect dest;

	dest.x = (Sint16)x;
	dest.y = (Sint16)y;
	/*other elements of dest are ignored by BlitSurface*/

	for (i = 0; i < TILE_LAYER_COUNT; ++i)
	{
		TileKind const * const layer = tile->layers[i];
		if (layer)
		{
			SDL_Surface * const image = images->images[layer->image_id];
			SDL_BlitSurface(image, 0, screen, &dest);
		}
	}
}

static void draw_tiles(
		Camera const *camera,
		SDL_Surface *screen,
		TileGrid const *tiles,
		ImageManager const *images)
{
	size_t const base_x = (size_t)camera->position.x - Width / 2;
	size_t const base_y = (size_t)camera->position.y - Height / 2;
	size_t const tile_width = 32;

	size_t ty;
	for (ty = 0; ty < tiles->height; ++ty)
	{
		size_t tx;
		for (tx = 0; tx < tiles->width; ++tx)
		{
			LayeredTile const * const tile = TileGrid_get(tiles, tx, ty);
			assert(tile);

			draw_layered_tile(
						(tx * tile_width) - base_x,
						(ty * tile_width) - base_y,
						screen,
						tile,
						images
						);
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
				case SDLK_ESCAPE:
					is_running = 0;
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
				   &sdl_front->game->current_map.terrain,
				   &sdl_front->images
				);

		SDL_Flip(screen);

		SDL_Delay(10);
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
	"sprites/grass_32.bmp",
	"sprites/dirt_32.bmp",
	"sprites/n_grass_s_dirt_32.bmp",
	"sprites/n_dirt_s_grass_32.bmp",
	"sprites/fence_32.bmp"
};

static int init_image_manager(ImageManager *images)
{
	size_t const image_count = sizeof(ImageFileNames) / sizeof(*ImageFileNames);
	size_t i;

	if (!ImageManager_init(images, image_count))
	{
		return 0;
	}

	for (i = 0; i < image_count; ++i)
	{
		SDL_Surface * const image = load_bmp_texture(
					ImageFileNames[i]
					);
		if (!image)
		{
			return 0;
		}
		images->images[i] = image;
	}

	return 1;
}

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

	if (TTF_Init() == -1)
	{
		fprintf(stderr, "SDL_tff error: %s\n", TTF_GetError());
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

	if (!init_image_manager(&front->images))
	{
		return 0;
	}

	return (Frontend *)front;
}
