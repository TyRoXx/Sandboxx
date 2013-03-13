#include "camera.hpp"
#include "world_tile_set.hpp"


namespace dungeons
{
	camera_position const &camera::get_position() const
	{
		return m_position;
	}

	void camera::set_position(camera_position const &position)
	{
		m_position = position;
	}

	void camera::render_world(
			world const &world,
			sdlpp::surface &canvas
			) const
	{
		//TODO implement
	}
}
