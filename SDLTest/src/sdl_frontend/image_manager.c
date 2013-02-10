#include "image_manager.h"
#include <assert.h>


int ImageManager_init(ImageManager *im, size_t image_count)
{
	assert(im);

	PtrVector_init(&im->images);
	return PtrVector_resize(&im->images, image_count);
}

static void free_image(void *image, void *user)
{
	(void)user;
	SDL_FreeSurface(image);
}

void ImageManager_free(ImageManager *im)
{
	for_each_ptr(PtrVector_begin(&im->images),
				 PtrVector_end(&im->images),
				 free_image, 0);
	PtrVector_free(&im->images);
}
