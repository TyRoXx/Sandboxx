#include "camera.hpp"
#include "world_tile_set.hpp"


namespace dungeons
{
	camera::camera(world_tile_set const &tile_set)
		: m_tile_set(tile_set)
	{
	}

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
