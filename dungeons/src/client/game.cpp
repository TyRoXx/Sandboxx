#include "game.hpp"


namespace dungeons
{
	void game::run()
	{
		while (m_view.tick())
		{
			sdlpp::delay(16);
		}
	}
}

