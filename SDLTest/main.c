#include "SDL.h"
#include <math.h>


typedef struct Tile
{
	SDL_Surface *image; /*owned*/
}
Tile;

void Tile_init(Tile *t, SDL_Surface *image)
{
	t->image = image;
}

void Tile_free(Tile *t)
{
	SDL_FreeSurface(t->image);
}


typedef struct TileGrid
{
	Tile const **tiles; /*malloc'ed*/
	size_t width, height;
}
TileGrid;

int TileGrid_init(TileGrid *g, size_t width, size_t height)
{
	g->tiles = calloc(width * height, sizeof(*g->tiles));
	g->width = width;
	g->height = height;
	return (g->tiles != 0);
}

void TileGrid_free(TileGrid *g)
{
	free(g->tiles);
}

Tile const *TileGrid_get(TileGrid const *g, size_t x, size_t y)
{
	return g->tiles[y * g->width + x];
}

void TileGrid_set(TileGrid const *g, size_t x, size_t y, Tile const *tile)
{
	g->tiles[y * g->width + x] = tile;
}


void draw_tiles(SDL_Surface *screen, size_t x, size_t y, TileGrid const *tiles)
{
	size_t ty;
	for (ty = 0; ty < tiles->height; ++ty)
	{
		size_t tx;
		for (tx = 0; tx < tiles->width; ++tx)
		{
			Tile const * const tile = TileGrid_get(tiles, tx, ty);
			SDL_Rect dest;

			if (!tile)
			{
				continue;
			}

			dest.x = x + (tx * tile->image->w);
			dest.y = y + (ty * tile->image->h);

			SDL_BlitSurface(tile->image, 0, screen, &dest);
		}
	}
}

enum
{
	Width = 640, Height = 480
};


int main(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		 fprintf(stderr, "SDL error: %s\n", SDL_GetError());
		 return 1;
	}

	{
		SDL_Surface * const screen = SDL_SetVideoMode(Width, Height, 32, SDL_SWSURFACE);
		SDL_Surface *grass, *dirt, *grass_dirt;
		Tile grass_tile, dirt_tile, grass_dirt_tile;
		TileGrid grid;
		SDL_Event event;
		int is_running = 1;
		size_t i;

		if (!screen)
		{
			fprintf(stderr, "Cannot create screen: %s\n", SDL_GetError());
			return 1;
		}

		grass = SDL_LoadBMP("sprites/grass32.bmp");
		dirt = SDL_LoadBMP("sprites/dirt32.bmp");
		grass_dirt = SDL_LoadBMP("sprites/grass_dirt32.bmp");
		if (!grass || !dirt || !grass_dirt)
		{
			return 1;
		}

		Tile_init(&grass_tile, grass);
		Tile_init(&dirt_tile, dirt);
		Tile_init(&grass_dirt_tile, grass_dirt);
		TileGrid_init(&grid, 12, 6);

		for (i = 0; i < 12; ++i)
		{
			TileGrid_set(&grid, i, 0, &grass_tile);
			TileGrid_set(&grid, i, 1, &grass_dirt_tile);
			TileGrid_set(&grid, i, 2, &dirt_tile);
		}

		while (is_running)
		{
			while (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT)
				{
					is_running = 0;
					break;
				}
			}

			if (screen->format->BytesPerPixel == 4)
			{
				unsigned x, y;
				unsigned const time_ms = SDL_GetTicks();

				for (y = 0; y < Height; ++y)
				{
					for (x = 0; x < Width; ++x)
					{
						uint32_t pixel = SDL_MapRGB(screen->format,
													(Uint8)((1 + sin((float)time_ms / 2800.0f)) * 128),
													(Uint8)((1 + cos((float)time_ms / 1700.0f)) * 128),
													(Uint8)((float)y / (float)Height * 256.0f));

						memcpy(((char *)screen->pixels) + ((Width * y) + x) * 4,
							   &pixel,
							   4);
					}
				}

				draw_tiles(screen, 64, 64, &grid);

				SDL_UpdateRect(screen, 0, 0, Width, Height);

				SDL_Delay(10);
			}
		}

		TileGrid_free(&grid);
		Tile_free(&grass_dirt_tile);
		Tile_free(&dirt_tile);
		Tile_free(&grass_tile);
	}

	SDL_Quit();
	return 0;
}
