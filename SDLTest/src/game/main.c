#include "base/game.h"
#include "sdl_frontend/sdl_frontend.h"


int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	{
		Game game;
		Frontend *frontend;

		if (!Game_init(&game))
		{
			return 1;
		}

		frontend = SDLFrontEnd_create(&game);
		if (!frontend)
		{
			return 1;
		}

		frontend->type->main_loop(frontend);
		frontend->type->destroy(frontend);

		Game_free(&game);
	}

	return 0;
}
