#include "world_screen.hpp"
#include "resources.hpp"


namespace dungeons
{
	world_screen::world_screen(view &view,
							   std::shared_ptr<resources> resources)
		: screen(view)
		, m_resources(std::move(resources))
		, m_camera(m_resources->world)
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
