#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "compiler.hpp"
#include "p0i/save_unit.hpp"
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
			(source_file_name.substr(source_file_name.find('.')) + ".p0i");

		p0::compiler compiler(
			"",
			p0::source_range(source.data(), source.data() + source.size())
			);

		p0::intermediate::unit const compiled_unit = compiler.compile();

		std::ofstream target_file(
			target_file_name,
			std::ios::binary);
		if (!target_file)
		{
			throw std::runtime_error("Could not open target file " + target_file_name);
		}

		p0::intermediate::save_unit(
			target_file,
			compiled_unit
			);
	}
	catch (const std::runtime_error &e)
	{
		cerr << e.what() << '\n';
	}
}
