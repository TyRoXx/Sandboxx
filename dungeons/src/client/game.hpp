#ifndef GAME_HPP
#define GAME_HPP


#include "sdl_display/view.hpp"
#include "client_logic/world.hpp"


namespace dungeons
{
	struct game
	{
		explicit game();
		void run();

	private:

		view m_view;
	};
}


#endif
