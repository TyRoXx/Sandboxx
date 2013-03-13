#include "screen.hpp"


namespace dungeons
{
	screen::screen(view &view)
		: m_view(view)
	{
	}

	screen::~screen()
	{
	}

	view &screen::get_view() const
	{
		return m_view;
	}

	void screen::on_event(SDL_Event const &)
	{
	}
}
