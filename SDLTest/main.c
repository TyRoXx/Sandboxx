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
		SDL_Event event;
		int is_running = 1;

		if (!screen)
		{
			fprintf(stderr, "Cannot create screen: %s\n", SDL_GetError());
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
				unsigned x, y;
				for (y = 0; y < Height; ++y)
				{
					for (x = 0; x < Width; ++x)
					{
						uint32_t pixel = SDL_MapRGB(screen->format,
													x % 256,
													y % 256,
													(x + y) % 256);

						memcpy(((char *)screen->pixels) + ((Width * y) + x) * 4,
							   &pixel,
							   4);
					}
				}
				SDL_UpdateRect(screen, 0, 0, Width, Height);
			}
		}
	}

	SDL_Quit();
	return 0;
}
