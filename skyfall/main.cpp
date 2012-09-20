#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <iostream>

namespace skyfall
{
	namespace
	{
		template <class I>
		void normalize(I begin, I end)
		{
			const auto sum = std::accumulate(begin, end, std::iterator_traits<I>::value_type(0));
			for (; begin != end; ++begin)
			{
				*begin /= sum;
			}
		}
	}


	struct element
	{
		element()
		{
		}

		virtual ~element()
		{
		}

		const sf::FloatRect &position() const
		{
			return m_position;
		}

		void position(const sf::FloatRect &position)
		{
			m_position = position;
			handle_resize();
		}

		virtual void handle_resize() = 0;
		virtual bool handle_event(const sf::Event &event) = 0;
		virtual void render(sf::RenderTarget &renderer) const = 0;
		virtual element *pick_child(const sf::Vector2f &point) = 0;

	private:

		sf::FloatRect m_position;
	};


	struct button : element
	{
		explicit button(std::string label, const sf::Font &font, unsigned font_size)
			: m_label(std::move(label))
			, m_font(font)
			, m_font_size(font_size)
		{
		}

		virtual void handle_resize()
		{
		}

		virtual bool handle_event(const sf::Event &event)
		{
			std::cout << m_label << "\n";
			return false;
		}

		virtual void render(sf::RenderTarget &renderer) const
		{
			sf::Text text(m_label, m_font, m_font_size);
			text.setPosition(position().left, position().top);
			text.setColor(sf::Color::Black);
			renderer.draw(text);
		}

		virtual element *pick_child(const sf::Vector2f &point)
		{
			return 0;
		}

	private:

		std::string m_label;
		const sf::Font &m_font;
		unsigned m_font_size;
	};


	struct grid : element
	{
		typedef std::vector<std::unique_ptr<element>> cells;
		typedef std::vector<float> relative_sizes;


		explicit grid(cells cells, size_t width, relative_sizes horizontal_sizes, relative_sizes vertical_sizes)
			: m_cells(std::move(cells))
			, m_width(width)
			, m_horizontal_sizes(std::move(horizontal_sizes))
			, m_vertical_sizes(std::move(vertical_sizes))
		{
			assert(m_cells.size() % width == 0);
			assert(width == m_horizontal_sizes.size());
			assert(height() == m_vertical_sizes.size());

			normalize(m_horizontal_sizes.begin(), m_horizontal_sizes.end());
			normalize(m_vertical_sizes.begin(), m_vertical_sizes.end());
		}

		size_t width() const
		{
			return m_width;
		}

		size_t height() const
		{
			return (m_cells.size() / width());
		}

		element &cell_at(size_t x, size_t y)
		{
			return *m_cells[y * m_width + x];
		}

		const element &cell_at(size_t x, size_t y) const
		{
			return *m_cells[y * m_width + x];
		}

		void recalculate_cells()
		{
			const auto grid_pos = position();
			float top = grid_pos.top;

			for (size_t y = 0; y < height(); ++y)
			{
				float left = grid_pos.left;
				const auto cell_height = (m_vertical_sizes[y] * grid_pos.height);

				for (size_t x = 0; x < width(); ++x)
				{
					const auto cell_width = (m_horizontal_sizes[x] * grid_pos.width);

					const sf::FloatRect cell_position(
						left,
						top,
						cell_width,
						cell_height
						);

					auto &cell = cell_at(x, y);
					cell.position(cell_position);

					left += cell_width;
				}

				top += cell_height;
			}
		}

		virtual void handle_resize()
		{
			recalculate_cells();
		}

		virtual bool handle_event(const sf::Event &event)
		{
			switch (event.type)
			{
			case sf::Event::MouseButtonPressed:
			case sf::Event::MouseButtonReleased:
			case sf::Event::MouseMoved:
			case sf::Event::MouseWheelMoved:
				break;
			}

			return false;
		}

		virtual void render(sf::RenderTarget &renderer) const
		{
			for (size_t y = 0; y < height(); ++y)
			{
				for (size_t x = 0; x < width(); ++x)
				{
					const auto &cell = cell_at(x, y);
					cell.render(renderer);
				}
			}
		}

		virtual element *pick_child(const sf::Vector2f &point)
		{
			sf::Vector2f current(position().left, position().top);
			size_t x = 0, y = 0;
			while (true)
			{
				current.x += m_horizontal_sizes[x] * position().width;
				if (current.x >= point.x)
				{
					break;
				}
				++x;
			}
			while (true)
			{
				current.y += m_vertical_sizes[y] * position().height;
				if (current.y >= point.y)
				{
					break;
				}
				++y;
			}
			return &cell_at(x, y);
		}

	private:

		cells m_cells;
		size_t m_width;
		relative_sizes m_horizontal_sizes;
		relative_sizes m_vertical_sizes;
	};


	struct window : element
	{
		explicit window(std::unique_ptr<element> content)
			: m_content(std::move(content))
		{
		}

		virtual void handle_resize()
		{
			m_content->position(position());
		}

		virtual bool handle_event(const sf::Event &event)
		{
			return m_content->handle_event(event);
		}

		virtual void render(sf::RenderTarget &renderer) const
		{
			m_content->render(renderer);
		}

		virtual element *pick_child(const sf::Vector2f &point)
		{
			return m_content.get();
		}

	private:

		std::unique_ptr<element> m_content;
	};


	struct environment
	{
		explicit environment(sf::RenderWindow &window, const sf::Font &label_font,
			const sf::Font &text_font)
			: m_window(window)
			, m_label_font(label_font)
			, m_text_font(text_font)
			, m_label_font_size(24)
			, m_text_font_size(16)
		{
			grid::cells cells(2 * 2);
			grid::relative_sizes horz(2), vert(2);
			horz[0] = 0.7f;
			horz[1] = 0.3f;
			vert[0] = 2;
			vert[1] = 5;
			cells[0].reset(new button("A", m_label_font, m_label_font_size));
			cells[1].reset(new button("B", m_label_font, m_label_font_size));
			cells[2].reset(new button("C", m_label_font, m_label_font_size));
			cells[3].reset(new button("D", m_label_font, m_label_font_size));

			m_test_window.reset(
				new skyfall::window(std::unique_ptr<element>(new grid(std::move(cells), 2, horz, vert))));
			m_test_window->position(sf::FloatRect(0, 0, 1200, 700));
		}

		void render()
		{
			m_window.clear(sf::Color::White);

			m_test_window->render(m_window);

			m_window.display();
		}

		void handle_event(const sf::Event &event)
		{
			switch (event.type)
			{
			case sf::Event::MouseButtonPressed:
			case sf::Event::MouseButtonReleased:
				{
					const sf::Vector2f point(
						static_cast<float>(event.mouseButton.x),
						static_cast<float>(event.mouseButton.y)
						);

					element *candidate = m_test_window.get();

					while (candidate &&
						candidate->position().contains(point) &&
						!candidate->handle_event(event))
					{
						candidate = candidate->pick_child(point);
					}
					break;
				}

			default:
				//to do focus
				break;
			}
		}

	private:

		sf::RenderWindow &m_window;
		const sf::Font &m_label_font;
		const sf::Font &m_text_font;
		unsigned m_label_font_size;
		unsigned m_text_font_size;
		std::unique_ptr<window> m_test_window;
	};
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(1200, 700), "Skyfall");
	window.setFramerateLimit(60);

	sf::Font label_font, text_font;

	if (!label_font.loadFromFile("CRYSRG__.TTF") ||
		!text_font.loadFromFile("DejaVuSerif.ttf"))
	{
		return 1;
	}

	skyfall::environment env(window, label_font, text_font);

	while (window.isOpen())
	{
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window.close();
				}
				else
				{
					env.handle_event(event);
				}
			}
		}

		env.render();
	}

	return 0;
}
