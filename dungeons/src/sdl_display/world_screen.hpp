#ifndef WORLD_SCREEN_HPP
#define WORLD_SCREEN_HPP


#include "screen.hpp"
#include "client_logic/world.hpp"


namespace dungeons
{
	struct world_screen : screen
	{
		explicit world_screen(view &view);
		virtual void on_event(SDL_Event const &event);

	private:

		world m_world;
	};
}


#endif
