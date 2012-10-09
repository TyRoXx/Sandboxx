#include "compiler.hpp"
#include "compiler_error.hpp"
#include "p0i/save_unit.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

namespace
{
	void print_help()
	{
		cout << "p0c [source file] [target file]\n";
	}

	std::vector<char> read_file(const std::string &path)
	{
		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			throw std::runtime_error("Could not open file " + path);
		}
		return std::vector<char>(
			std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>()
			);
	}

	bool print_error_return_true(
		p0::source_range source,
		size_t &error_counter,
		const p0::compiler_error &error
		)
	{
		++error_counter;

		auto const pos = error.position();
		size_t const line_index = std::count(
			source.begin(),
			pos.begin(),
			'\n'); //TODO O(n) instead of O(n^2)

		auto const end_of_line = std::find(pos.begin(), source.end(), '\n');
		auto const line_length = std::distance(pos.begin(), end_of_line);
		auto const hint_length = std::min<size_t>(70, line_length);

		std::string const line(
			pos.begin(),
			pos.begin() + hint_length
			);

		cerr << '(' << (line_index + 1) << "): " << error.what() << '\n';
		cerr << "    " << line << '\n';
		return true;
	}

	std::string make_output_file_name(
		std::string const &input_file_name
		)
	{
		return std::string(
			input_file_name.begin(),
			std::find(input_file_name.begin(), input_file_name.end(), '.')
			) +
			".p0i";
	}
}

int main(int argc, char **argv)
{
	if (argc <= 1)
	{
		print_help();
		return 0;
	}
	
	try
	{
		std::string const source_file_name = argv[1];
		auto const source = read_file(source_file_name);
		std::string const target_file_name = (argc >= 3) ?
			argv[2] :
			make_output_file_name(source_file_name);

		p0::source_range source_range(
			source.data(),
			source.data() + source.size());

		size_t error_counter = 0;
		auto const handle_error = std::bind(print_error_return_true,
			source_range,
			std::ref(error_counter),
			std::placeholders::_1
			);

		p0::compiler compiler(
			source_range,
			handle_error
			);

		p0::intermediate::unit const compiled_unit = compiler.compile();

		if (error_counter)
		{
			cerr << error_counter << " error";
			if (error_counter != 1)
			{
				cerr << "s";
			}
			cerr << "\n";
			return 1;
		}

		std::ofstream target_file(
			target_file_name,
			std::ios::binary
			);
		if (!target_file)
		{
			throw std::runtime_error("Could not open target file " + target_file_name);
		}

		p0::intermediate::save_unit(
			target_file,
			compiled_unit
			);
	}
	catch (std::runtime_error const &e)
	{
		cerr << e.what() << '\n';
		return 1;
	}
}
