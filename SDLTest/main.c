#include "SDL.h"

enum
{
	Width = 640, Height = 480
};

int main(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		 fprintf(stderr, "SDL error: %s\n", SDL_GetError());
		 return 1;
	}

	{
		SDL_Surface * const screen = SDL_SetVideoMode(Width, Height, 32, SDL_SWSURFACE);
		SDL_Surface * grass;
		SDL_Event event;
		int is_running = 1;

		if (!screen)
		{
			fprintf(stderr, "Cannot create screen: %s\n", SDL_GetError());
			return 1;
		}

		grass = SDL_LoadBMP("sprites/grass32.bmp");
		if (!grass)
		{
			return 1;
		}

		while (is_running)
		{
			SDL_WaitEvent(&event);
			switch (event.type)
			{
			case SDL_QUIT:
				is_running = 0;
				break;
			}

			if (screen->format->BytesPerPixel == 4)
			{
				SDL_Rect dest, source;

				unsigned x, y;
				for (y = 0; y < Height; ++y)
				{
					for (x = 0; x < Width; ++x)
					{
						uint32_t pixel = SDL_MapRGB(screen->format,
													44,
													44,
													(Uint8)((float)y / (float)Height * 256.0f));

						memcpy(((char *)screen->pixels) + ((Width * y) + x) * 4,
							   &pixel,
							   4);
					}
				}

				dest.x = 128;
				dest.y = 128;
			//	dest.w = grass->w;
			//	dest.h = grass->h;

				source.x = 0;
				source.y = 0;
				source.w = grass->w;
				source.h = grass->h;

				SDL_BlitSurface(grass, &source, screen, &dest);

				SDL_UpdateRect(screen, 0, 0, Width, Height);
			}
		}
	}

	SDL_Quit();
	return 0;
}
