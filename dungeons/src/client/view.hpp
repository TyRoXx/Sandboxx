#ifndef VIEW_HPP
#define VIEW_HPP


#include "sdlpp/sdlpp.hpp"
#include <memory>


namespace dungeons
{
	struct view
	{
		view();
		bool tick();

	private:

		sdlpp::window m_window;
	};
}


#endif
