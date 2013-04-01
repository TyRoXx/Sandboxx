#ifndef DATA_H
#define DATA_H


#include "image_manager.h"


typedef struct Data
{
	ImageManager images;
}
Data;


Bool Data_init(Data *d, char const *directory, SDL_PixelFormat *format);
void Data_free(Data *d);


#endif
