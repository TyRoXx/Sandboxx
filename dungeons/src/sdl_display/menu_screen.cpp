#include "menu_screen.hpp"
#include "view.hpp"
#include "world_screen.hpp"
#include "shared/shared.hpp"


namespace dungeons
{
	menu_screen::menu_screen(view &view)
		: screen(view)
	{
	}

	void menu_screen::on_event(SDL_Event const &event)
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			get_view().switch_screen(make_unique<world_screen>(get_view()));
			return;

		default:
			break;
		}
	}
}
