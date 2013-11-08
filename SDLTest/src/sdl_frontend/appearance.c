#include "appearance.h"
#include "image_manager.h"
#include "base/algorithm.h"
#include <assert.h>
#include <jansson.h>


Bool AnimationSide_init(AnimationSide *a, size_t frame_count)
{
	a->frames = malloc(frame_count * sizeof(AnimationFrame));
	a->frame_count = frame_count;
	return (a->frames != NULL);
}

void AnimationSide_free(AnimationSide *a)
{
	free(a->frames);
}


void Animation_free(Animation *a)
{
	size_t i;
	for (i = 0; i < DIR_COUNT; ++i)
	{
		AnimationSide_free(&a->sides[i]);
	}
}


void AppearanceLayout_free(AppearanceLayout *a)
{
	size_t i;

	for (i = 0; i < (size_t)Anim_COUNT; ++i)
	{
		Animation_free(a->animations + i);
	}
}


void Appearance_init(Appearance *appearance,
					 SDL_Surface *image,
					 AppearanceLayout const *layout)
{
	appearance->image = image;
	appearance->layout = layout;
}

void Appearance_free(Appearance *appearance)
{
	(void)appearance;
}


unsigned const tile_size = 32;

static Bool init_animation(
		Animation *anim,
		AnimationType anim_id,
		Bool (*init_side)(AnimationSide *, AnimationType, Direction))
{
	Bool result = True;
	size_t i;

	for (i = 0; i < DIR_COUNT; ++i)
	{
		AnimationSide * const side = &anim->sides[i];
		if (!init_side(side, anim_id, (Direction)i))
		{
			result = False;
			break;
		}
	}

	if (!result)
	{
		size_t c;
		for (c = i, i = 0; i < c; ++i)
		{
			AnimationSide_free(&anim->sides[i]);
		}
	}

	return result;
}

static Bool init_layout(AppearanceLayout *layout,
						Bool (*init_side)(AnimationSide *, AnimationType, Direction))
{
	Bool result = True;
	size_t i, c;
	Vector2i offset;
	offset.x = 0;
	offset.y = 0;

	for (i = 0, c = (size_t)Anim_COUNT; i < c; ++i)
	{
		Animation * const anim = layout->animations + i;

		if (!init_animation(anim, (AnimationType)i, init_side))
		{
			result = False;
			break;
		}

		offset.y += (ptrdiff_t)(DIR_COUNT * tile_size);
	}

	if (!result)
	{
		for (c = i, i = 0; i < c; ++i)
		{
			Animation_free(layout->animations + i);
		}
		return False;
	}

	return True;
}

static Bool init_dynamic_side_1(AnimationSide *side,
								AnimationType anim_id,
								Direction side_id)
{
	size_t const frame_count = 3;
	size_t j;
	SDL_Rect section;

	if (!AnimationSide_init(side, frame_count))
	{
		return False;
	}

	section.x = 0;
	section.y = (Sint16)(((unsigned)anim_id * DIR_COUNT + (unsigned)side_id) * tile_size);
	section.w = (Uint16)tile_size;
	section.h = (Uint16)tile_size;

	for (j = 0; j < frame_count; ++j)
	{
		AnimationFrame * const frame = side->frames + j;
		frame->duration = 200;
		frame->section = section;

		section.x = (Sint16)((unsigned)section.x + tile_size);
	}

	return True;
}

static Bool init_static_side(AnimationSide *side,
							 AnimationType anim_id,
							 Direction side_id)
{
	SDL_Rect section;

	(void)anim_id;
	(void)side_id;

	if (!AnimationSide_init(side, 1))
	{
		return False;
	}

	section.x = (Sint16)0;
	section.y = (Sint16)0;
	section.w = (Uint16)tile_size;
	section.h = (Uint16)tile_size;

	side->frames->duration = 0;
	side->frames->section = section;
	return True;
}

static Bool init_dynamic_layout_1(AppearanceLayout *layout)
{
	return init_layout(layout, &init_dynamic_side_1);
}

static Bool init_static_layout(AppearanceLayout *layout)
{
	return init_layout(layout, &init_static_side);
}

static Bool add_appearance(AppearanceManager *a,
						   ImageManager *images,
						   char const *image_name,
						   AppearanceLayout const *layout)
{
	SDL_Surface *image;
	Appearance appearance;

	image = ImageManager_get(images, image_name);
	if (!image)
	{
		return False;
	}

	Appearance_init(&appearance, image, layout);
	if (Vector_push_back(&a->appearances, &appearance, sizeof(appearance)))
	{
		return True;
	}
	Appearance_free(&appearance);
	return False;
}

static void free_appearance(void *appearance, void *user)
{
	(void)user;
	Appearance_free(appearance);
}

static void free_appearances(Vector *appearances)
{
	for_each(Vector_begin(appearances),
			 Vector_end(appearances),
			 sizeof(Appearance),
			 free_appearance,
			 NULL);
	Vector_free(appearances);
}

Bool AppearanceManager_init(AppearanceManager *a)
{
	if (init_static_layout(&a->static_layout))
	{
		if (init_dynamic_layout_1(&a->dynamic_layout_1))
		{
			Vector_init(&a->appearances);
			return True;
		}
		AppearanceLayout_free(&a->static_layout);
	}
	AppearanceManager_free(a);
	return False;
}

static Bool parse_appearances(
        json_t *array,
        Bool (*on_appearance)(size_t, char const *, char const *, void *),
        void *user)
{
	size_t i, c;
	for (i = 0, c = json_array_size(array); i < c; ++i)
	{
		json_t * const element = json_array_get(array, i);
		json_t const * id = json_object_get(element, "id");
		json_t const * type = json_object_get(element, "type");
		json_t const * file = json_object_get(element, "file");
		char const *type_str;
		char const *file_str;
		if (!id || !type || !file)
		{
			fprintf(stderr, "Appearance entry %lu is incomplete\n", (unsigned long)i);
			return False;
		}
		if (!json_is_integer(id) || ((size_t)json_integer_value(id) != i))
		{
			fprintf(stderr, "Unexpected id in appearance entry %lu\n", (unsigned long)i);
			return False;
		}
		type_str = json_string_value(type);
		if (!type_str)
		{
			fprintf(stderr, "The type of appearance %lu is expected to be specified as a string\n", (unsigned long)i);
			return False;
		}
		file_str = json_string_value(file);
		if (!file_str)
		{
			fprintf(stderr, "The file of appearance %lu is expected to be specified as a string\n", (unsigned long)i);
			return False;
		}
		if (!on_appearance(i, type_str, file_str, user))
		{
			return False;
		}
	}
	return True;
}

typedef struct parse_appearances_args
{
	AppearanceManager *a;
	struct ImageManager *images;
}
parse_appearances_args;

static Bool on_appearance(size_t index, char const *type, char const *file, void *user)
{
	parse_appearances_args const * const args = user;
	AppearanceManager * const a = args->a;
	AppearanceLayout const *layout = NULL;
	if (!strcmp("static", type))
	{
		layout = &a->static_layout;
	}
	else if (!strcmp("dynamic1", type))
	{
		layout = &a->dynamic_layout_1;
	}
	if (!layout)
	{
		fprintf(stderr, "Appearance %lu has unknown layout %s\n", (unsigned long)index, type);
		return False;
	}
	return add_appearance(args->a, args->images, file, layout);
}

Bool AppearanceManager_parse_file(
        AppearanceManager *a,
        char const *begin,
        size_t length,
        struct ImageManager *images)
{
	json_error_t error;
	json_t * const root = json_loadb(begin, length, 0, &error);
	if (!root)
	{
		fprintf(stderr, "Error on line %d: %s\n", error.line, error.text);
		return False;
	}

	{
		Bool result;
		if (json_is_array(root))
		{
			parse_appearances_args args;
			args.a = a;
			args.images = images;
			result = parse_appearances(root, on_appearance, &args);
		}
		else
		{
			fprintf(stderr, "Expected a global array in appearances JSON file\n");
			result = False;
		}
		json_decref(root);
		return result;
	}
}

void AppearanceManager_free(AppearanceManager *a)
{
	free_appearances(&a->appearances);
	AppearanceLayout_free(&a->dynamic_layout_1);
	AppearanceLayout_free(&a->static_layout);
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
