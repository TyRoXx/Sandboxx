#ifndef SDLPP_HPP
#define SDLPP_HPP


#include <SDL/SDL.h>
#include <SDL_ttf.h>
#include <array>
#include <stdexcept>
#include <cstdint>
#include <boost/optional.hpp>


namespace sdlpp
{
	template <class T, std::size_t N>
	struct vector
	{
		std::array<T, N> elements;

		vector()
		{
			elements.fill(T{0});
		}

		template <class ...Elements>
		vector(Elements ...elements)
			: elements{{elements...}}
		{
		}
	};

	template <class T, std::size_t N>
	vector<T, N> &operator += (vector<T, N> &left, vector<T, N> const &right)
	{
		for (std::size_t i = 0; i < N; ++i)
		{
			left.elements[i] += right.elements[i];
		}
		return left;
	}

	template <class T, std::size_t N>
	vector<T, N> &operator -= (vector<T, N> &left, vector<T, N> const &right)
	{
		return left += (-right);
	}

	template <class T, std::size_t N>
	vector<T, N> operator - (vector<T, N> const &v)
	{
		return vector<T, N>() -= v;
	}

	template <class To, class From, std::size_t N>
	vector<To, N> vector_cast(vector<From, N> const &from)
	{
		vector<To, N> to;
		for (std::size_t i = 0; i < N; ++i)
		{
			to.elements[i] = static_cast<To>(from.elements[i]);
		}
		return to;
	}

	typedef vector<unsigned, 2> vector2u;

	template <class T>
	T width(vector<T, 2> const &size)
	{
		return size.elements[0];
	}

	template <class T>
	T height(vector<T, 2> const &size)
	{
		return size.elements[1];
	}

	struct color
	{
		SDL_Color value;

		color();
		color(SDL_Color value);
	};

	struct sdl_error : std::runtime_error
	{
		sdl_error();
	};

	///throws sdlpp::error if result < 0
	void check_sdl_result(int result);

	struct surface
	{
		surface();
		surface(surface &&other);
		surface(SDL_Surface &owned);
		~surface();
		surface &operator = (surface &&other);
		void swap(surface &other);
		void set_color_key(color key);
		surface convert(SDL_PixelFormat &format, std::uint32_t flags);
		SDL_Surface *handle();
		SDL_PixelFormat &format();
		void fill(std::uint32_t color, SDL_Rect &area);
		void fill(std::uint32_t color);
		void blit(SDL_Rect *dest_area, SDL_Surface &source, SDL_Rect *source_area);
		vector2u get_size() const;

	private:

		SDL_Surface *m_handle;
	};

	surface load_bitmap_file(char const *file_name);
	surface load_bitmap_memory(char const *data, std::size_t length);


	struct sdl_initializer
	{
		sdl_initializer();
		~sdl_initializer();

	private:

		sdl_initializer(sdl_initializer const &);
		sdl_initializer &operator = (sdl_initializer const &);
	};

	struct window
	{
		window(vector2u size, bool is_double_buffered);
		void set_caption(char const *caption);
		boost::optional<SDL_Event> poll_event();
		SDL_Event wait_for_event();
		void flip();
		surface &screen();

	private:

		sdl_initializer m_initializer;
		surface m_screen;
	};

	struct ttf_error : std::runtime_error
	{
		ttf_error();
	};

	void check_ttf_result(int result);

	struct ttf_initializer
	{
		explicit ttf_initializer(window &);
		~ttf_initializer();

	private:

		ttf_initializer(ttf_initializer const &);
		ttf_initializer &operator = (ttf_initializer const &);
	};

	void delay(std::uint32_t milliseconds);
	std::uint32_t get_ticks();
}


#endif
