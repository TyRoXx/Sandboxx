#include "view.hpp"


namespace dungeons
{
	view::view()
		: m_window(sdlpp::vector2u(800, 600), true)
	{
		m_window.set_caption("dungeons");
	}

	bool view::tick()
	{
		bool is_running = true;
		boost::optional<SDL_Event> ev;
		for (;;)
		{
			ev = m_window.poll_event();
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

		m_window.screen().fill(0xff00ff);

		m_window.flip();
		return is_running;
	}
}
