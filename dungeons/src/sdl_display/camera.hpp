#ifndef CAMERA_HPP
#define CAMERA_HPP


#include "sdlpp/sdlpp.hpp"


namespace dungeons
{
	struct world;
	struct world_tile_set;


	typedef sdlpp::vector<ptrdiff_t, 2> camera_position;


	struct camera
	{
		explicit camera(world_tile_set const &tile_set);
		camera_position const &get_position() const;
		void set_position(camera_position const &position);
		void render_world(
				world const &world,
				sdlpp::surface &canvas
				) const;

	private:

		camera_position m_position;
		world_tile_set const &m_tile_set;
	};
}


#endif
