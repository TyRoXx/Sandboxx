#include "data.h"
#include "path.h"
#include <assert.h>


#define USE_APPEARANCES_V2 1


static Bool init_appearances(
		AppearanceManager *appearances,
		char const *data_directory,
		ImageManager *images)
{
	char * const appearance_file_name = join_paths(data_directory,
#if USE_APPEARANCES_V2
												   "appearances2.txt"
#else
	                                               "appearances.txt"
#endif
	                                               );
	FILE *file;
	Bool result;

	if (!appearance_file_name)
	{
		return False;
	}

	file = fopen(appearance_file_name,
#if USE_APPEARANCES_V2
	             "rb"
#else
	             "r"
#endif
	             );
	if (!file)
	{
		fprintf(stderr, "Could not open file %s\n", appearance_file_name);
		free(appearance_file_name);
		return False;
	}

	free(appearance_file_name);
#if USE_APPEARANCES_V2
	result = AppearanceManager_init2(appearances);
	if (result)
	{
		Vector content;
		Vector_init(&content);
		result = Vector_append_binary_file(&content, file);
		if (result)
		{
			result = AppearanceManager_parse_file_v2(appearances, Vector_data(&content), Vector_size(&content), images);
		}
		Vector_free(&content);
	}
#else
	result = AppearanceManager_init(appearances, file, images);
#endif
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
