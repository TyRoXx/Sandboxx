#ifndef APPEARANCE_H
#define APPEARANCE_H


#include "texture_ref.h"
#include "base/entity.h"
#include <SDL.h>


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

typedef struct Animation
{
	AnimationFrame *frames;
	size_t frame_count;
}
Animation;

Bool Animation_init(Animation *a, size_t frame_count);
void Animation_free(Animation *a);
void Animation_set_frame(Animation *a, size_t index, AnimationFrame frame);

typedef struct Appearance
{
	AppearanceId id;
	Animation animations[Anim_COUNT];
}
Appearance;


#endif
