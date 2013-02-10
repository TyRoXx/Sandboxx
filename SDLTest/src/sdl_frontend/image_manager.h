#ifndef IMAGE_MANAGER_H
#define IMAGE_MANAGER_H


#include "SDL.h"
#include "base/ptr_vector.h"


typedef struct ImageManager
{
	PtrVector images;
}
ImageManager;

int ImageManager_init(ImageManager *im, size_t image_count);
void ImageManager_free(ImageManager *im);


#endif
