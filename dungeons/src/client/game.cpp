#include "game.hpp"
#include "sdl_display/menu_screen.hpp"
#include "shared/shared.hpp"


namespace dungeons
{
	game::game()
	{
		m_view.switch_screen(make_unique<menu_screen>(m_view));
	}

	void game::run()
	{
		while (m_view.tick())
		{
			sdlpp::delay(16);
		}
	}
}

