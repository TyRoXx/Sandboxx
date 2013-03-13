#ifndef WORLD_SCREEN_HPP
#define WORLD_SCREEN_HPP


#include "client_logic/world.hpp"
#include "screen.hpp"
#include "camera.hpp"


namespace dungeons
{
	struct world_screen : screen
	{
		explicit world_screen(view &view);
		virtual void on_event(SDL_Event const &event);
		virtual void render(sdlpp::surface &canvas) const;

	private:

		world m_world;
		camera m_camera;
	};
}


#endif
