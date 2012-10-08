#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "compiler.hpp"
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
		std::string const unit_name = (argc >= 3) ?
			argv[2] :
		source_file_name.substr(source_file_name.find('.'));

		p0::compiler compiler(
			unit_name,
			p0::compiler::source_range(source.data(), source.data() + source.size())
			);

		p0::intermediate::unit const compiled_unit = compiler.compile();
	}
	catch (const std::runtime_error &e)
	{
		cerr << e.what() << '\n';
	}
}
