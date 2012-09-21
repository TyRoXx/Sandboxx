#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <iostream>
#include <boost/signals2.hpp>

#ifdef _MSC_VER
#define SKYFALL_OVERRIDE override
#else
#define SKYFALL_OVERRIDE
#endif

namespace skyfall
{
	namespace
	{
		template <class I>
		void normalize_to_sum_1(I begin, I end)
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
		virtual bool query_size(sf::Vector2f &size) const = 0;

	private:

		sf::FloatRect m_position;
	};


	struct frame : element
	{
		explicit frame(std::unique_ptr<element> content)
			: m_content(std::move(content))
		{
		}

		virtual void handle_resize() SKYFALL_OVERRIDE
		{
			m_content->position(sf::FloatRect(
				position().left - m_position_in_content.x,
				position().top - m_position_in_content.y,
				position().width,
				position().height));
		}

		virtual bool handle_event(const sf::Event &event) SKYFALL_OVERRIDE
		{
			return false;
		}

		virtual void render(sf::RenderTarget &renderer) const SKYFALL_OVERRIDE
		{
			m_content->render(renderer);
		}

		virtual element *pick_child(const sf::Vector2f &point) SKYFALL_OVERRIDE
		{
			const auto point_in_content = (point - m_position_in_content);
			if (m_content->position().contains(point_in_content))
			{
				return m_content.get();
			}

			return 0;
		}

		virtual bool query_size(sf::Vector2f &size) const SKYFALL_OVERRIDE
		{
			return m_content->query_size(size);
		}

	private:

		std::unique_ptr<element> m_content;
		sf::Vector2f m_position_in_content;
	};


	struct button : element
	{
		boost::signals2::signal<void ()> clicked;


		explicit button(std::string label, const sf::Font &font, unsigned font_size)
			: m_label(std::move(label))
			, m_font(font)
			, m_font_size(font_size)
		{
		}

		virtual void handle_resize() SKYFALL_OVERRIDE
		{
		}

		virtual bool handle_event(const sf::Event &event) SKYFALL_OVERRIDE
		{
			switch (event.type)
			{
			case sf::Event::MouseButtonReleased:
				clicked();
				return true;

			default:
				return false;
			}
		}

		virtual void render(sf::RenderTarget &renderer) const SKYFALL_OVERRIDE
		{
			sf::RectangleShape background(sf::Vector2f(position().width - 10, position().height - 10));
			background.setPosition(sf::Vector2f(position().left + 5, position().top + 5));
			background.setFillColor(sf::Color(127, 127, 127, 127));
			background.setOutlineColor(sf::Color::Black);
			background.setOutlineThickness(2);
			renderer.draw(background);

			sf::Text text(m_label, m_font, m_font_size);
			text.setPosition(position().left, position().top);
			text.setColor(sf::Color::Black);
			renderer.draw(text);
		}

		virtual element *pick_child(const sf::Vector2f &point) SKYFALL_OVERRIDE
		{
			return 0;
		}

		virtual bool query_size(sf::Vector2f &size) const SKYFALL_OVERRIDE
		{
			size = measure_label();
			return true;
		}

	private:

		std::string m_label;
		const sf::Font &m_font;
		unsigned m_font_size;


		sf::Vector2f measure_label() const
		{
			sf::Vector2f size;
			
			for (auto i = m_label.begin(); i != m_label.end(); ++i)
			{
				const auto c = *i;
				const auto &glyph = m_font.getGlyph(c, m_font_size, false);
				size.x += static_cast<float>(glyph.bounds.width);
			}

			size.y = static_cast<float>(m_font_size);
			return size;
		}
	};


	struct grid : element
	{
		typedef std::vector<std::unique_ptr<element>> cells;
		typedef std::vector<float> relative_sizes;
		typedef sf::Vector2i gravity_vector;


		explicit grid(cells cells, size_t width, relative_sizes horizontal_sizes, relative_sizes vertical_sizes, gravity_vector gravity = gravity_vector(0, 0))
			: m_cells(std::move(cells))
			, m_width(width)
			, m_horizontal_sizes(std::move(horizontal_sizes))
			, m_vertical_sizes(std::move(vertical_sizes))
			, m_gravity(gravity)
			
		{
			assert(m_cells.size() % width == 0);
			assert(width == m_horizontal_sizes.size());
			assert(height() == m_vertical_sizes.size());

			normalize_to_sum_1(m_horizontal_sizes.begin(), m_horizontal_sizes.end());
			normalize_to_sum_1(m_vertical_sizes.begin(), m_vertical_sizes.end());
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

		void recalculate_cells2()
		{
			const auto grid_pos = position();

			std::vector<float> x_desired_sizes(width(), -1), y_desired_sizes(height(), -1);

			for (size_t y = 0; y < height(); ++y)
			{
				for (size_t x = 0; x < width(); ++x)
				{
					auto &cell = cell_at(x, y);

					sf::Vector2f cell_size;
					if (cell.query_size(cell_size))
					{
					}
					else
					{
						cell_size = sf::Vector2f();
					}

					auto &column_width = x_desired_sizes[x];
					column_width = std::max(column_width, cell_size.x);

					auto &row_height = y_desired_sizes[y];
					row_height = std::max(row_height, cell_size.y);
				}
			}

			const float desired_width = std::accumulate(x_desired_sizes.begin(), x_desired_sizes.end(), 0);
			const float desired_height = std::accumulate(y_desired_sizes.begin(), y_desired_sizes.end(), 0);
			const float x_factor = (position().width / desired_width);
			const float y_factor = (position().height / desired_height);

			std::vector<float> x_positions(width()), y_positions(height());
			float x_pos = 0, y_pos = 0;

			for (size_t i = 0; i < x_desired_sizes.size(); ++i)
			{
				const auto desired_size = x_desired_sizes[i];
				x_positions[i] = x_pos;

				switch (m_gravity.x)
				{
				case 0:
					{
						const auto real_size = (desired_size * x_factor);
						x_pos += real_size;
						break;
					}
				}
			}

			for (size_t i = 0; i < y_desired_sizes.size(); ++i)
			{
				const auto desired_size = y_desired_sizes[i];
				y_positions[i] = y_pos;

				switch (m_gravity.y)
				{
				case 0:
					{
						const auto real_size = (desired_size * y_factor);
						y_pos += real_size;
						break;
					}
				}
			}

			for (size_t y = 0; y < height(); ++y)
			{
				for (size_t x = 0; x < width(); ++x)
				{
					auto &cell = cell_at(x, y);

					const auto cell_width  = (x == (width()  - 1) ? (x_pos - x_positions[x]) : (x_positions[x + 1] - x_positions[x]));
					const auto cell_height = (y == (height() - 1) ? (y_pos - y_positions[y]) : (y_positions[y + 1] - y_positions[y]));

					cell.position(sf::FloatRect(
						position().left + x_positions[x],
						position().top  + y_positions[y],
						cell_width,
						cell_height
						));
				}
			}
		}

		virtual void handle_resize() SKYFALL_OVERRIDE
		{
			recalculate_cells2();
		}

		virtual bool handle_event(const sf::Event &event) SKYFALL_OVERRIDE
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

		virtual void render(sf::RenderTarget &renderer) const SKYFALL_OVERRIDE
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

		virtual element *pick_child(const sf::Vector2f &point) SKYFALL_OVERRIDE
		{
			sf::Vector2f current(position().left, position().top);
			size_t x = 0;
			for (;;)
			{
				current.x += m_horizontal_sizes[x] * position().width;
				if (current.x >= point.x)
				{
					break;
				}
				++x;
			}
			size_t y = 0;
			for (;;)
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

		virtual bool query_size(sf::Vector2f &size) const SKYFALL_OVERRIDE
		{
			sf::Vector2f calculated_size;
			bool result = false;

			for (size_t y = 0; y < height(); ++y)
			{
				float row_width = 0;

				for (size_t x = 0; x < width(); ++x)
				{
					const auto &cell = cell_at(x, y);

					sf::Vector2f cell_size;
					if (cell.query_size(cell_size))
					{
						result = true;

						calculated_size.y += cell_size.y;
						row_width += cell_size.x;
					}
				}

				if (row_width > calculated_size.x)
				{
					calculated_size.x = row_width;
				}
			}

			if (result)
			{
				size = calculated_size;
			}
			return result;
		}

	private:

		cells m_cells;
		size_t m_width;
		relative_sizes m_horizontal_sizes;
		relative_sizes m_vertical_sizes;
		gravity_vector m_gravity;
	};


	struct window : element
	{
		explicit window(std::unique_ptr<element> content)
			: m_content(std::move(content))
		{
		}

		virtual void handle_resize() SKYFALL_OVERRIDE
		{
			m_content->position(position());
		}

		virtual bool handle_event(const sf::Event &event) SKYFALL_OVERRIDE
		{
			return m_content->handle_event(event);
		}

		virtual void render(sf::RenderTarget &renderer) const SKYFALL_OVERRIDE
		{
			m_content->render(renderer);
		}

		virtual element *pick_child(const sf::Vector2f &point) SKYFALL_OVERRIDE
		{
			return m_content.get();
		}

		virtual bool query_size(sf::Vector2f &size) const SKYFALL_OVERRIDE
		{
			return m_content->query_size(size);
		}

	private:

		std::unique_ptr<element> m_content;
	};


	struct image : element
	{
		explicit image(const sf::Texture &texture)
			: m_texture(texture)
		{
		}

		virtual void handle_resize() SKYFALL_OVERRIDE
		{
		}

		virtual bool handle_event(const sf::Event &event) SKYFALL_OVERRIDE
		{
			return false;
		}

		virtual void render(sf::RenderTarget &renderer) const SKYFALL_OVERRIDE
		{
			sf::Sprite sprite(m_texture);
			sprite.setPosition(position().left, position().top);
			renderer.draw(sprite);
		}

		virtual element *pick_child(const sf::Vector2f &point) SKYFALL_OVERRIDE
		{
			return 0;
		}

		virtual bool query_size(sf::Vector2f &size) const SKYFALL_OVERRIDE
		{
			const auto texture_size = m_texture.getSize();
			size.x = static_cast<float>(texture_size.x);
			size.y = static_cast<float>(texture_size.y);
			return true;
		}

	private:

		const sf::Texture &m_texture;
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
			m_test_texture.loadFromFile("test.png");

			grid::cells cells(2 * 2);
			grid::relative_sizes horz(2), vert(2);
			horz[0] = 0.7f;
			horz[1] = 0.3f;
			vert[0] = 2;
			vert[1] = 5;
			cells[0].reset(new button("A", m_label_font, m_label_font_size));
			cells[1].reset(new button("B", m_label_font, m_label_font_size));
			cells[2].reset(new button("C", m_label_font, m_label_font_size));

			{
				const size_t image_count = 12;
				grid::cells images(image_count);
				for (size_t i = 0; i < images.size(); ++i)
				{
					images[i].reset(new image(m_test_texture));
				}
				std::unique_ptr<grid> image_grid(new grid(
					std::move(images), 1, grid::relative_sizes(1, 1), grid::relative_sizes(image_count, 1)));
				cells[3].reset(new frame(std::move(image_grid)));
			}

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
		sf::Texture m_test_texture;
	};
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(1200, 700), "Skyfall", sf::Style::Close);
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
				else if (event.type == sf::Event::Resized)
				{
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
