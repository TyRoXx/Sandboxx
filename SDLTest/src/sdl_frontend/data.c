#include "data.h"
#include "path.h"
#include <assert.h>


static Bool init_appearances(
		AppearanceManager *appearances,
		char const *data_directory,
		ImageManager *images)
{
	char * const appearance_file_name = join_paths(data_directory,
												   "appearances.txt");
	FILE *file;
	Bool result;

	if (!appearance_file_name)
	{
		return False;
	}

	file = fopen(appearance_file_name, "r");
	if (!file)
	{
		fprintf(stderr, "Could not open file %s\n", appearance_file_name);
		free(appearance_file_name);
		return False;
	}

	free(appearance_file_name);
	result = AppearanceManager_init(appearances, file, images);
	fclose(file);
	return result;
}

Bool Data_init(Data *d,
			   char const *directory,
			   SDL_PixelFormat *format)
{
	char *image_directory = join_paths(directory, "sprites");
	if (!image_directory)
	{
		return False;
	}

	ImageManager_init(&d->images, image_directory, format);

	if (!init_appearances(&d->appearances, directory, &d->images))
	{
		ImageManager_free(&d->images);
		return False;
	}

	return True;
}

void Data_free(Data *d)
{
	AppearanceManager_free(&d->appearances);
	ImageManager_free(&d->images);
}
