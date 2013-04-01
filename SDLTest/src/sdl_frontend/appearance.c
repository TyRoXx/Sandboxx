#include "appearance.h"
#include "image_manager.h"
#include "base/algorithm.h"
#include <assert.h>


Bool Animation_init(Animation *a, size_t frame_count)
{
	a->frames = malloc(frame_count * sizeof(AnimationFrame));
	a->frame_count = frame_count;
	return (a->frames != NULL);
}

void Animation_free(Animation *a)
{
	free(a->frames);
}


void Appearance_free(Appearance *appearance)
{
	size_t i;

	for (i = 0; i < (size_t)Anim_COUNT; ++i)
	{
		Animation_free(appearance->animations + i);
	}
}

static Bool add_static_appearance(
	AppearanceManager *a,
	ImageManager *images,
	char const *image_name)
{
	SDL_Surface * const surface = ImageManager_get(images, image_name);
	Appearance appearance;
	size_t i, c;
	Bool result = True;

	if (!surface)
	{
		return False;
	}

	for (i = 0, c = (size_t)Anim_COUNT; i < c; ++i)
	{
		Animation *anim = appearance.animations + i;
		AnimationFrame *frame;

		if (!Animation_init(anim, 1))
		{
			result = False;
			break;
		}

		frame = anim->frames + 0;
		frame->duration = 0;
		frame->texture = TextureRef_full(surface);
	}

	if (!result)
	{
		for (c = i, i = 0; i < c; ++i)
		{
			Animation_free(appearance.animations + i);
		}
		return False;
	}

	if (!Vector_push_back(&a->appearances, &appearance, sizeof(appearance)))
	{
		Appearance_free(&appearance);
		return False;
	}

	return True;
}

static Bool load_appearances_file(AppearanceManager *a,
								  FILE *file,
								  ImageManager *images)
{
	unsigned expected_index = 0;
	unsigned index;
	char type[32];

	while (fscanf(file, " %u %31s", &index, type) == 2)
	{
		if (index != expected_index)
		{
			fprintf(stderr, "Expected appearance index %u, found %u\n",
					expected_index, index);
			return False;
		}

		if (!strcmp("STATIC", type))
		{
			char image_name[1024];
			if (fscanf(file, " %1023s", image_name) != 1)
			{
				fprintf(stderr, "Static appearance image name expected\n");
				return False;
			}

			if (!add_static_appearance(a, images, image_name))
			{
				return False;
			}
		}
		else
		{
			fprintf(stderr, "Unknown appearance type\n");
		}

		++expected_index;
	}

	return True;
}

Bool AppearanceManager_init(AppearanceManager *a,
							FILE *file,
							ImageManager *images)
{
	Vector_init(&a->appearances);

	if (load_appearances_file(a, file, images))
	{
		return True;
	}

	AppearanceManager_free(a);
	return False;
}

static void free_appearance(void *appearance, void *user)
{
	(void)user;
	Appearance_free(appearance);
}

void AppearanceManager_free(AppearanceManager *a)
{
	for_each(Vector_begin(&a->appearances),
			 Vector_end(&a->appearances),
			 sizeof(Appearance),
			 free_appearance,
			 NULL);
	Vector_free(&a->appearances);
}

Appearance const *AppearanceManager_get(AppearanceManager const *a,
										AppearanceId id)
{
	size_t const offset = id * sizeof(Appearance);
	if (offset >= Vector_size(&a->appearances))
	{
		return NULL;
	}
	return (Appearance *)(Vector_data(&a->appearances) + offset);
}
