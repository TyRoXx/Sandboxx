#ifndef IMAGE_MANAGER_H
#define IMAGE_MANAGER_H


#include "SDL.h"
#include "base/vector.h"


typedef struct Image
{
	char *name;
	SDL_Surface *surface;
}
Image;

void Image_init(Image *im, char *name, SDL_Surface *surface);
void Image_free(Image *im);


typedef struct ImageManager
{
	char *directory;
	SDL_PixelFormat *format;
	Vector images;
}
ImageManager;

void ImageManager_init(ImageManager *im,
					   char *directory,
					   SDL_PixelFormat *format);
void ImageManager_free(ImageManager *im);
SDL_Surface *ImageManager_get(ImageManager *im, char const *name);


#endif
