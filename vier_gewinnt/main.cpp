#include <iostream>
#include <bitset>
#include <vector>
#include <string>
#include <functional>
#include <array>
#include <cassert>
#include "console.hpp"

namespace vg
{
	enum cell_possession
	{
		red,
		green,
		nobody,
	};

	template <class T>
	struct coordinates
	{
		typedef std::array<T, 2> type;
	};

	typedef coordinates<size_t>::type vectoru;
	typedef coordinates<ptrdiff_t>::type vectori;

	enum
	{
		x,
		y,
	};

	template <class T>
	typename coordinates<T>::type make_vector(T x, T y)
	{
		typename coordinates<T>::type result = {{x, y}};
		return result;
	}

	template <class T>
	void add(typename coordinates<T>::type &left, typename coordinates<T>::type right)
	{
		for (size_t i = 0; i < left.size(); ++i)
		{
			left[i] += right[i];
		}
	}

	template <class T, class U>
	typename coordinates<T>::type convert_vector(U source)
	{
		typename coordinates<T>::type result;
		for (size_t i = 0; i < result.size(); ++i)
		{
			result[i] = static_cast<T>(source[i]);
		}
		return result;
	}

	template <class T>
	void negate(std::array<T, 2> &vector)
	{
		for (size_t i = 0; i < vector.size(); ++i)
		{
			vector[i] = -vector[i];
		}
	}


	struct field_state
	{
		typedef std::vector<cell_possession> cell_vector;


		cell_vector cells;
		std::size_t width;


		explicit field_state(vectoru size)
			: cells(size[x] * size[y], nobody)
			, width(size[x])
		{
		}

		size_t get_height() const
		{
			return (cells.size() / width);
		}

		size_t position_to_index(vectoru position) const
		{
			return position[x] + position[y] * width;
		}

		cell_possession get_cell(vectoru position) const
		{
			return cells[position_to_index(position)];
		}

		void set_cell(vectoru position, cell_possession cell)
		{
			cells[position_to_index(position)] = cell;
		}
	};


	bool is_inside(
		vectori size,
		vectori point)
	{
		for (size_t i = 0; i < size.size(); ++i)
		{
			if (point[i] < 0 ||
				point[i] >= size[i])
			{
				return false;
			}
		}

		return true;
	}

	size_t count_streak_after(
		const field_state &field,
		vectori begin,
		vectori increment
		)
	{
		const auto streak_color = field.get_cell(convert_vector<size_t>(begin));
		assert(streak_color != nobody);

		const auto field_size = make_vector<ptrdiff_t>(field.width, field.get_height());

		size_t streak = 0;

		for (;;)
		{
			add<ptrdiff_t>(begin, increment);

			if (!is_inside(field_size, begin))
			{
				break;
			}

			if (field.get_cell(convert_vector<size_t>(begin)) != streak_color)
			{
				break;
			}

			++streak;
		}

		return streak;
	}

	bool has_row(
		const field_state &field,
		size_t row_length,
		vectori position,
		vectori direction)
	{
		const auto positive = count_streak_after(field, position, direction);
		negate(direction);
		const auto negative = count_streak_after(field, position, direction);
		const auto streak = (1 + positive + negative);
		return (streak >= row_length);
	}

	bool is_winning_turn(
		const field_state &field,
		size_t row_length,
		vectoru position
		)
	{
		const std::array<vectori, 3> directions =
		{
			make_vector<ptrdiff_t>(1, 0),
			make_vector<ptrdiff_t>(1, 1),
			make_vector<ptrdiff_t>(0, 1),
		};

		const auto position_i = convert_vector<ptrdiff_t>(position);

		return std::any_of(directions.begin(), directions.end(),
			std::bind(has_row,
			std::ref(field),
			row_length,
			position_i,
			std::placeholders::_1));
	}

	bool exists_turn(
		const field_state &field)
	{
		for (size_t x = 0; x < field.width; ++x)
		{
			if (field.get_cell(make_vector<size_t>(x, 0)) == nobody)
			{
				return true;
			}
		}

		return false;
	}

	size_t insert_coin(field_state &field, unsigned column, cell_possession color)
	{
		assert(color != nobody);

		const auto height = field.get_height();

		size_t y = 0;
		for (; y < height; ++y)
		{
			if (field.get_cell(make_vector(column, y)) != nobody)
			{
				if (y == 0)
				{
					throw std::runtime_error("Cannot insert into full column");
				}

				break;
			}
		}

		--y;
		field.set_cell(make_vector(column, y), color);
		return y;
	}


	struct player
	{
		typedef std::function<unsigned (const field_state &)> choose_column_t;


		std::string name;
		choose_column_t choose_column;


		explicit player(
			std::string name,
			choose_column_t choose_column)
			: name(std::move(name))
			, choose_column(std::move(choose_column))
		{
		}
	};


	player *play(
		player *two_players,
		const std::function<void (const field_state &)> &field_changed
		)
	{
		field_state field(make_vector<size_t>(7, 6));
		field_changed(field);

		for (bool turn = false; exists_turn(field); turn = !turn)
		{
			player &current = two_players[turn];

			const size_t chosen_column = current.choose_column(field);
			const size_t coin_y = insert_coin(field, chosen_column, turn ? red : green);

			field_changed(field);

			if (is_winning_turn(field, 4, make_vector(chosen_column, coin_y)))
			{
				return &current;
			}
		}

		return 0;
	}

	char get_cell_char(cell_possession cell)
	{
		switch (cell)
		{
		case red: return 'x';
		case green: return 'o';
		default: return ' ';
		}
	}

	void set_cell_console_color(cell_possession cell)
	{
		console_color foreground;

		switch (cell)
		{
		case red:
			foreground = Red;
			break;

		case green:
			foreground = Green;
			break;

		default:
			return;
		}

		set_console_color(foreground, Black);
	}

	void set_console_frame_color()
	{
		set_console_color(White, Black);
	}

	void render_field(
		std::ostream &out,
		const field_state &field)
	{
		clear_console();
		set_console_frame_color();

		const auto width = field.width;
		const auto height = field.get_height();
		const auto horizontal_bar = std::string(width * 2 + 1, '-');

		out << horizontal_bar << '\n';

		for (size_t y = 0; y < height; ++y)
		{
			out << '|';

			for (size_t x = 0; x < width; ++x)
			{
				const auto cell = field.get_cell(make_vector(x, y));
				set_cell_console_color(cell);
				out << get_cell_char(cell);

				if (x != (width - 1))
				{
					out << ' ';
				}

				set_console_frame_color();
			}

			out << "|\n";
		}

		out << horizontal_bar << '\n';

		out << ' ';
		for (size_t x = 0; x < width; ++x)
		{
			out << static_cast<char>('0' + (x % 10)) << ' ';
		}
		out << '\n';
	}

	bool check_column_input(const field_state &field, size_t column)
	{
		if (column >= field.width)
		{
			std::cout << "Diese Spalte gibt es nicht\n";
			return false;
		}

		if (field.get_cell(make_vector<size_t>(column, 0)) != nobody)
		{
			std::cout << "Diese Spalte ist schon voll\n";
			return false;
		}

		return true;
	}

	size_t let_player_choose_column(const field_state &field)
	{
		size_t column = 0;
		auto &in = std::cin;

		for (;;)
		{
			in >> column;

			if (!in)
			{
				in.clear();
				in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

				std::cout << "Bitte eine Zahl von 0 bis " << (field.width - 1) << " eingeben\n";
				continue;
			}

			if (!check_column_input(field, column))
			{
				continue;
			}

			break;
		}

		return column;
	}

	size_t let_computer_choose_column(const field_state &field)
	{
		size_t x = 0;
		for (; x < field.width; ++x)
		{
			if (field.get_cell(make_vector<size_t>(x, 0)) == nobody)
			{
				break;
			}
		}
		return x;
	}
}

int main()
{
	using namespace vg;

	player players[2] =
	{
		player("player", let_player_choose_column),
		player("computer", let_computer_choose_column)
	};

	auto &out = std::cout;

	player * const winner = play(
		players,
		std::bind(render_field, std::ref(out), std::placeholders::_1));

	if (winner)
	{
		out << winner->name << " gewinnt\n";
	}
	else
	{
		out << "Das Spiel endet unentschieden\n";
	}
}
