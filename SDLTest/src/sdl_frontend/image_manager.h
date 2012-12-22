#ifndef IMAGE_MANAGER_H
#define IMAGE_MANAGER_H


#include "SDL.h"
#include <stddef.h>


typedef struct ImageManager
{
	SDL_Surface **images;
	size_t image_count;
}
ImageManager;

int ImageManager_init(ImageManager *im, size_t image_count);
void ImageManager_free(ImageManager *im);


#endif
