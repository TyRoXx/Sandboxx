#ifndef WORLD_SCREEN_HPP
#define WORLD_SCREEN_HPP


#include "client_logic/world.hpp"
#include "screen.hpp"
#include "camera.hpp"
#include <memory>


namespace dungeons
{
	struct resources;


	struct world_screen : screen
	{
		explicit world_screen(view &view,
							  std::shared_ptr<resources> resources);
		virtual void on_event(SDL_Event const &event);
		virtual void render(sdlpp::surface &canvas) const;

	private:

		world m_world;
		std::shared_ptr<resources> const m_resources;
		camera m_camera;
	};
}


#endif
