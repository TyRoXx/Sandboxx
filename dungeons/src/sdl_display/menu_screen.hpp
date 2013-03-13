#ifndef MENU_SCREEN_HPP
#define MENU_SCREEN_HPP


#include "screen.hpp"


namespace dungeons
{
	struct menu_screen : screen
	{
		explicit menu_screen(view &view);
		virtual void on_event(SDL_Event const &event);

	private:
	};
}


#endif
