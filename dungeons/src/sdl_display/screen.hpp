#ifndef SCREEN_HPP
#define SCREEN_HPP


#include "sdlpp/sdlpp.hpp"


namespace dungeons
{
	struct view;


	struct screen
	{
		explicit screen(view &view);
		virtual ~screen();
		view &get_view() const;
		virtual void on_event(SDL_Event const &event);
		virtual void render(sdlpp::surface &canvas) const = 0;

	private:

		view &m_view;
	};
}


#endif
