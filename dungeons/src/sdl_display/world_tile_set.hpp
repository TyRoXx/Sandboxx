#ifndef WORLD_TILE_SET_HPP
#define WORLD_TILE_SET_HPP


#include "sdlpp/sdlpp.hpp"
#include <vector>


namespace dungeons
{
	struct world_tile_set
	{
		std::vector<sdlpp::surface> environment;
	};
}


#endif
