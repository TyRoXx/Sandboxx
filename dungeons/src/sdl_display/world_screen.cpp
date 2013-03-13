#include "world_screen.hpp"


namespace dungeons
{
	world_screen::world_screen(view &view)
		: screen(view)
	{
	}

	void world_screen::on_event(SDL_Event const &)
	{
	}

	void world_screen::render(sdlpp::surface &canvas) const
	{
		//TODO fill with bg color if necessary

		m_camera.render_world(m_world, canvas);

		//TODO render gui
	}
}
