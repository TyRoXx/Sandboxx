#ifndef TEXTURE_REF_H
#define TEXTURE_REF_H


#include <SDL.h>


typedef struct TextureRef
{
	SDL_Surface *surface;
	SDL_Rect section;
}
TextureRef;

TextureRef TextureRef_full(SDL_Surface *surface);


#endif
