#include "view.hpp"


namespace dungeons
{
	view::view()
		: m_window(sdlpp::vector2u(800, 600), true)
		, m_is_running(true)
	{
		m_window.set_caption("dungeons");
	}

	bool view::tick()
	{
		boost::optional<SDL_Event> ev;
		for (;;)
		{
			ev = m_window.poll_event();
			if (!ev)
			{
				break;
			}

			handle_event(*ev);
		}

		m_window.screen().fill(0xff00ff);

		m_window.flip();
		return m_is_running;
	}

	void view::switch_screen(std::unique_ptr<screen> screen)
	{
		m_screen = std::move(screen);
	}


	void view::handle_event(SDL_Event const &event)
	{
		switch (event.type)
		{
		case SDL_QUIT:
			m_is_running = false;
			return;

		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				m_is_running = false;
				return;

			default:
				break;
			}

		default:
			break;
		}

		if (m_screen)
		{
			m_screen->on_event(event);
		}
	}
}
