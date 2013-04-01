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
	unsigned duration;
	TextureRef texture;
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
	AnimationSide sides[4];
}
Animation;

void Animation_free(Animation *a);


typedef struct Appearance
{
	Animation animations[Anim_COUNT];
}
Appearance;

void Appearance_free(Appearance *appearance);

typedef struct AppearanceManager
{
	Vector appearances;
}
AppearanceManager;

Bool AppearanceManager_init(AppearanceManager *a,
							FILE *file,
							struct ImageManager *images);
void AppearanceManager_free(AppearanceManager *a);
Appearance const *AppearanceManager_get(AppearanceManager const *a,
										AppearanceId id);


#endif
