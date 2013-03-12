#ifndef SCREEN_HPP
#define SCREEN_HPP


#include "SDL/SDL.h"


namespace dungeons
{
	struct view;


	struct screen
	{
		explicit screen(view &view);
		virtual ~screen();
		void suspend();
		void resume();
		virtual void on_event(SDL_Event const &event);

	private:

		view &m_view;


		virtual void on_suspend();
		virtual void on_resume();
	};
}


#endif
