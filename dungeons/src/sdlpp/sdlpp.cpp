#include "sdlpp.hpp"


namespace sdlpp
{
	color::color()
	{
	}

	color::color(SDL_Color value)
		: value(value)
	{
	}


	sdl_error::sdl_error()
		: std::runtime_error(SDL_GetError())
	{
	}

	void check_sdl_result(int result)
	{
		if (result < 0)
		{
			throw sdl_error();
		}
	}

	surface::surface()
		: m_handle(nullptr)
	{
	}

	surface::surface(surface &&other)
		: m_handle(nullptr)
	{
		swap(other);
	}

	surface::surface(SDL_Surface &owned)
		: m_handle(&owned)
	{
	}

	surface::~surface()
	{
		SDL_FreeSurface(m_handle);
	}

	surface &surface::operator = (surface &&other)
	{
		swap(other);
		return *this;
	}

	void surface::swap(surface &other)
	{
		std::swap(m_handle, other.m_handle);
	}

	void surface::set_color_key(color key)
	{
		check_sdl_result(SDL_SetColorKey(
						 m_handle,
						 SDL_SRCCOLORKEY,
						 SDL_MapRGB(m_handle->format, key.value.r, key.value.g, key.value.b)));
	}

	surface surface::convert(SDL_PixelFormat &format, std::uint32_t flags)
	{
		SDL_Surface * const converted =
				SDL_ConvertSurface(m_handle, &format, flags);
		if (!converted)
		{
			throw sdl_error();
		}
		return surface(*converted);
	}

	SDL_Surface *surface::handle()
	{
		return m_handle;
	}

	SDL_PixelFormat &surface::format()
	{
		return *m_handle->format;
	}

	void surface::fill(std::uint32_t color, SDL_Rect &area)
	{
		check_sdl_result(SDL_FillRect(m_handle, &area, color));
	}

	void surface::fill(std::uint32_t color)
	{
		check_sdl_result(SDL_FillRect(m_handle, nullptr, color));
	}

	void surface::blit(SDL_Rect *dest_area, SDL_Surface &source, SDL_Rect *source_area)
	{
		check_sdl_result(SDL_BlitSurface(&source, source_area, m_handle, dest_area));
	}


	surface load_bitmap_file(char const *file_name)
	{
		SDL_Surface * const loaded = SDL_LoadBMP(file_name);
		if (!loaded)
		{
			throw sdl_error();
		}
		return surface(*loaded);
	}

	surface load_bitmap_memory(char const *data, std::size_t length)
	{
		SDL_Surface * const loaded =
				SDL_LoadBMP_RW(SDL_RWFromConstMem(data, length), true);
		if (!loaded)
		{
			throw sdl_error();
		}
		return surface(*loaded);
	}


	sdl_initializer::sdl_initializer()
	{
		check_sdl_result(SDL_Init(SDL_INIT_VIDEO));
	}

	sdl_initializer::~sdl_initializer()
	{
		SDL_Quit();
	}


	namespace
	{
		SDL_Surface &open_window(vector2u size, bool is_double_buffered)
		{
			SDL_Surface * const screen = SDL_SetVideoMode(
						width(size),
						height(size),
						32,
						is_double_buffered ? SDL_DOUBLEBUF : 0);
			if (!screen)
			{
				throw sdl_error();
			}
			return *screen;
		}
	}

	window::window(vector2u size, bool is_double_buffered)
		: m_screen(open_window(size, is_double_buffered))
	{
	}

	void window::set_caption(char const *caption)
	{
		SDL_WM_SetCaption(caption, caption);
	}

	boost::optional<SDL_Event> window::poll_event()
	{
		SDL_Event event;
		if (SDL_PollEvent(&event))
		{
			return event;
		}
		return {};
	}

	SDL_Event window::wait_for_event()
	{
		SDL_Event event;
		if (!SDL_WaitEvent(&event))
		{
			throw sdl_error();
		}
		return event;
	}

	void window::flip()
	{
		check_sdl_result(SDL_Flip(m_screen.handle()));
	}

	surface &window::screen()
	{
		return m_screen;
	}


	ttf_error::ttf_error()
		: std::runtime_error(TTF_GetError())
	{
	}

	void check_ttf_result(int result)
	{
		if (result < 0)
		{
			throw ttf_error();
		}
	}

	ttf_initializer::ttf_initializer(window &)
	{
		check_ttf_result(TTF_Init());
	}

	ttf_initializer::~ttf_initializer()
	{
		TTF_Quit();
	}

	void delay(std::uint32_t milliseconds)
	{
		SDL_Delay(milliseconds);
	}

	std::uint32_t get_ticks()
	{
		return SDL_GetTicks();
	}
}
