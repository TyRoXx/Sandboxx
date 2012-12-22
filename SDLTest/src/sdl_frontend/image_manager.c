#include "image_manager.h"
#include <assert.h>


int ImageManager_init(ImageManager *im, size_t image_count)
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

void ImageManager_free(ImageManager *im)
{
	size_t i;
	for (i = 0; i < im->image_count; ++i)
	{
		SDL_Surface * const s = im->images[i];
		SDL_FreeSurface(s);
	}
	free(im->images);
}
