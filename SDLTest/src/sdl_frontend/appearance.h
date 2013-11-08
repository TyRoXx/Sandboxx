#ifndef APPEARANCE_H
#define APPEARANCE_H


#include "texture_ref.h"
#include "base/entity.h"
#include "base/vector.h"
#include <SDL.h>


struct ImageManager;

typedef enum AnimationType
{
	Anim_Idle,
	Anim_Move,
	Anim_Die,
	Anim_Dead,
	Anim_COUNT
}
AnimationType;

typedef struct AnimationFrame
{
	SDL_Rect section;
	unsigned duration;
}
AnimationFrame;

typedef struct AnimationSide
{
	AnimationFrame *frames;
	size_t frame_count;
}
AnimationSide;

Bool AnimationSide_init(AnimationSide *a, size_t frame_count);
void AnimationSide_free(AnimationSide *a);


typedef struct Animation
{
	AnimationSide sides[DIR_COUNT];
}
Animation;

void Animation_free(Animation *a);


typedef struct AppearanceLayout
{
	Animation animations[Anim_COUNT];
}
AppearanceLayout;


void AppearanceLayout_free(AppearanceLayout *a);


typedef struct Appearance
{
	SDL_Surface *image;
	AppearanceLayout const *layout;
}
Appearance;

void Appearance_init(Appearance *appearance,
					 SDL_Surface *image,
					 AppearanceLayout const *layout);
void Appearance_free(Appearance *appearance);

typedef struct AppearanceManager
{
	AppearanceLayout static_layout;
	AppearanceLayout dynamic_layout_1;
	Vector appearances;
}
AppearanceManager;

Bool AppearanceManager_init(AppearanceManager *a);
Bool AppearanceManager_parse_file(
        AppearanceManager *a,
        char const *begin,
        size_t length,
        struct ImageManager *images);
void AppearanceManager_free(AppearanceManager *a);
Appearance const *AppearanceManager_get(AppearanceManager const *a,
										AppearanceId id);


#endif
