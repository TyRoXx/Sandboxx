#include "texture_ref.h"


TextureRef TextureRef_full(SDL_Surface *surface)
{
	TextureRef ref;
	SDL_Rect dimensions;
	ref.surface = surface;
	ref.section.x = ref.section.y = 0;
	SDL_GetClipRect(surface, &dimensions);
	ref.section.w = dimensions.w;
	ref.section.h = dimensions.h;
	return ref;
}
