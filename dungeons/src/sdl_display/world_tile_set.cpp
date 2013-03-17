#include "world_tile_set.hpp"


namespace dungeons
{
	world_tile_set::world_tile_set(std::vector<std::string> const &environment_files)
	{
		for (auto &name : environment_files)
		{
			environment.push_back(sdlpp::load_bitmap_file(name.c_str()));
		}
	}
}
