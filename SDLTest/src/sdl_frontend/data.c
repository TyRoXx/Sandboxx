#include "data.h"
#include <assert.h>


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

static Bool init_image_manager(ImageManager *images,
							  SDL_PixelFormat *format)
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

Bool Data_init(Data *d,
			   char const *directory,
			   SDL_PixelFormat *format)
{
	(void)directory; /*TODO*/
	return init_image_manager(&d->images, format);
}

void Data_free(Data *d)
{
	ImageManager_free(&d->images);
}
