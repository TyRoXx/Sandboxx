#include <iostream>
#include <bitset>
#include <vector>
#include <string>
#include <functional>
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


	struct field_state
	{
		typedef std::vector<cell_possession> cell_vector;


		cell_vector cells;
		std::size_t width;


		field_state(std::size_t width, std::size_t height)
			: cells(width * height, nobody)
			, width(width)
		{
		}

		size_t get_height() const
		{
			return (cells.size() / width);
		}

		cell_possession get_cell(size_t x, size_t y) const
		{
			return cells[x + y * width];
		}

		void set_cell(size_t x, size_t y, cell_possession cell)
		{
			cells[x + y * width] = cell;
		}
	};


	bool is_winning_turn(
		const field_state &field,
		size_t row_length,
		size_t x,
		size_t y)
	{
		return false;
	}

	bool exists_turn(
		const field_state &field)
	{
		for (size_t x = 0; x < field.width; ++x)
		{
			if (field.get_cell(x, 0) == nobody)
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
			if (field.get_cell(column, y) != nobody)
			{
				if (y == 0)
				{
					throw std::runtime_error("Cannot insert into full column");
				}

				break;
			}
		}

		--y;
		field.set_cell(column, y, color);
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
		field_state field(7, 6);
		field_changed(field);

		for (bool turn = false; exists_turn(field); turn = !turn)
		{
			player &current = two_players[turn];

			const size_t chosen_column = current.choose_column(field);
			const size_t coin_y = insert_coin(field, chosen_column, turn ? red : green);

			field_changed(field);

			if (is_winning_turn(field, 4, chosen_column, coin_y))
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
				const auto cell = field.get_cell(x, y);
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
		if (field.get_cell(column, 0) != nobody)
		{
			std::cout << "Diese Spalte ist schon voll\n";
			return true;
		}

		return false;
	}

	size_t let_player_choose_column(const field_state &field)
	{
		size_t column = 0;
		do
		{
			std::cin >> column;
		}
		while (check_column_input(field, column));
		return column;
	}

	size_t let_computer_choose_column(const field_state &field)
	{
		size_t x = 0;
		for (; x < field.width; ++x)
		{
			if (field.get_cell(x, 0) == nobody)
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
		out << "unentschieden\n";
	}
}
