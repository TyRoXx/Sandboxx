#ifndef VIEW_HPP
#define VIEW_HPP


#include "screen.hpp"
#include "sdlpp/sdlpp.hpp"
#include <memory>


namespace dungeons
{
	struct view
	{
		explicit view();
		bool tick();
		void switch_screen(std::unique_ptr<screen> screen);

	private:

		sdlpp::window m_window;
		std::unique_ptr<screen> m_screen;
		bool m_is_running;


		void handle_event(SDL_Event const &event);
	};
}


#endif
