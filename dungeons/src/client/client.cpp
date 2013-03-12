#include "client.hpp"
#include "sdlpp/sdlpp.hpp"


namespace dungeons
{
}

int main(int argc, char **argv)
{
	sdlpp::window window(sdlpp::vector2u(800, 600), true);
	window.set_caption("dungeons");

	bool is_running = true;
	while (is_running)
	{
		boost::optional<SDL_Event> ev;
		for (;;)
		{
			ev = window.poll_event();
			if (!ev)
			{
				break;
			}

			switch (ev->type)
			{
			case SDL_QUIT:
				is_running = false;
				break;

			case SDL_KEYUP:
				switch (ev->key.keysym.sym)
				{
				case SDLK_ESCAPE:
					is_running = false;
					break;

				default:
					break;
				}

			default:
				break;
			}
		}

		window.screen().fill(0xff00ff);

		window.flip();

		sdlpp::delay(16);
	}
}
