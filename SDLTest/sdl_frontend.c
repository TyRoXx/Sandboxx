#include "sdl_frontend.h"
#include "game.h"
#include "vector2f.h"
#include "minmax.h"
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
	assert(im);

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

static SDL_Surface *load_bmp_texture(char const *file_name, SDL_PixelFormat *format)
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
			SDL_MapRGB(converted->format,	AlphaKey.r,	AlphaKey.g,	AlphaKey.b)) < 0)
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

	ImageManager_free(&sdl_front->images);
	free(front);

	TTF_Quit();
	SDL_Quit();
}

static void draw_entity(
	ptrdiff_t px,
	ptrdiff_t py,
	SDL_Surface *screen,
	Entity const *entity,
	ImageManager const *images)
{
	SDL_Surface * const image = images->images[entity->appearance.tile_set_id];
	SDL_Rect dest;
	dest.x = (Sint16)px;
	dest.y = (Sint16)py;
	SDL_BlitSurface(image, 0, screen, &dest);
}

static void draw_entities(
	Camera const *camera,
	SDL_Surface *screen,
	World const *world,
	size_t tile_width,
	ImageManager const *images)
{
	size_t i;

	assert(world);
	assert(camera);
	assert(screen);
	assert(images);

	for (i = 0; i < world->entity_count; ++i)
	{
		Entity const * const entity = world->entities + i;
		draw_entity(
			(ptrdiff_t)((float)(entity->x - camera->position.x) * tile_width + (float)Width / 2),
			(ptrdiff_t)((float)(entity->y - camera->position.y) * tile_width + (float)Height / 2),
			screen,
			entity,
			images);
	}
}

static void draw_layered_tile(
	ptrdiff_t px,
	ptrdiff_t py,
	SDL_Surface *screen,
	LayeredTile const *tile,
	ImageManager const *images
)
{
	size_t i;
	for (i = 0; i < TILE_LAYER_COUNT; ++i)
	{
		TileKind const * const layer = tile->layers[i];
		if (layer)
		{
			SDL_Surface * const image = images->images[layer->image_id];
			SDL_Rect dest;
			dest.x = (Sint16)px;
			dest.y = (Sint16)py;
			/*Other elements of dest are ignored by SDL_BlitSurface.*/
			/*dest is not reused because SDL_BlitSurface may modify it.*/

			SDL_BlitSurface(image, 0, screen, &dest);
		}
	}
}

static void draw_tiles(
		Camera const *camera,
		SDL_Surface *screen,
		TileGrid const *tiles,
		size_t tile_width,
		ImageManager const *images)
{
	ptrdiff_t y;

	ptrdiff_t visible_begin_idx = (ptrdiff_t)(camera->position.x - (float)Width  / (float)tile_width / 2.0f);
	ptrdiff_t visible_begin_idy = (ptrdiff_t)(camera->position.y - (float)Height / (float)tile_width / 2.0f);
	ptrdiff_t visible_end_idx   = (ptrdiff_t)(camera->position.x + (float)Width  / (float)tile_width / 2.0f + 0.5f);
	ptrdiff_t visible_end_idy   = (ptrdiff_t)(camera->position.y + (float)Height / (float)tile_width / 2.0f + 0.5f);

	visible_begin_idx = max_ptrdiff_t(visible_begin_idx, 0);
	visible_begin_idy = max_ptrdiff_t(visible_begin_idy, 0);
	visible_end_idx   = min_ptrdiff_t(visible_end_idx,   (ptrdiff_t)tiles->width);
	visible_end_idy   = min_ptrdiff_t(visible_end_idy,   (ptrdiff_t)tiles->height);

	for (y = visible_begin_idy; y < visible_end_idy; ++y)
	{
		ptrdiff_t x;
		for (x = visible_begin_idx; x < visible_end_idx; ++x)
		{
			LayeredTile const * const tile = TileGrid_get(tiles, (size_t)x, (size_t)y);
			assert(tile);

			draw_layered_tile(
						(ptrdiff_t)((float)tile_width * ((float)x - camera->position.x) + Width  / 2.0f),
						(ptrdiff_t)((float)tile_width * ((float)y - camera->position.y) + Height / 2.0f),
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

		if (SDL_MUSTLOCK(screen))
		{
			if (SDL_LockSurface(screen) < 0)
			{
				return;
			}
		}

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

		if (SDL_MUSTLOCK(screen))
		{
			SDL_UnlockSurface(screen);
		}
	}
}

enum
{
	TileWidth = 32
};

static void SDLFrontend_main_loop(Frontend *front)
{
	SDLFrontend * const sdl_front = (SDLFrontend *)front;
	SDL_Surface * const screen = sdl_front->screen;
	int is_running = 1;
	unsigned last_time = SDL_GetTicks();

	while (is_running)
	{
		unsigned current_time;
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
					sdl_front->camera.position.x -= 1;
					break;
				case SDLK_RIGHT:
					sdl_front->camera.position.x += 1;
					break;
				case SDLK_UP:
					sdl_front->camera.position.y -= 1;
					break;
				case SDLK_DOWN:
					sdl_front->camera.position.y += 1;
					break;
				case SDLK_ESCAPE:
					is_running = 0;
					break;
				default:
					break;
				}
			}
		}

		current_time = SDL_GetTicks();
		assert(current_time >= last_time);
		Game_update(sdl_front->game, (current_time - last_time));
		last_time = current_time;

		draw_background(screen);

		draw_tiles(&sdl_front->camera,
				   screen,
				   &sdl_front->game->current_map.terrain,
				   TileWidth,
				   &sdl_front->images
				);

		draw_entities(
			&sdl_front->camera,
			screen,
			&sdl_front->game->world,
			TileWidth,
			&sdl_front->images
			);

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
	"sprites/grass_32.bmp",
	"sprites/dirt_32.bmp",
	"sprites/n_grass_s_dirt_32.bmp",
	"sprites/n_dirt_s_grass_32.bmp",
	"sprites/fence_32.bmp",
	"sprites/nw_dirt_on_grass_32.bmp",
	"sprites/w_dirt_e_grass_32.bmp",
	"sprites/sw_dirt_on_grass_32.bmp",
};

static int init_image_manager(ImageManager *images, SDL_PixelFormat *format)
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
					ImageFileNames[i],
					format
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
	front->screen = SDL_SetVideoMode(Width, Height, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	front->camera.position.x = 6.5f;
	front->camera.position.y = 3.5f;

	if (!front->screen)
	{
		fprintf(stderr, "Cannot create screen: %s\n", SDL_GetError());
		return 0;
	}

	if (!init_image_manager(&front->images, front->screen->format))
	{
		return 0;
	}

	return (Frontend *)front;
}
