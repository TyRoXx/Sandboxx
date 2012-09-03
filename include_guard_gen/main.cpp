#include <random>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>

namespace
{
	void generateGuardName(
		std::ostream &destination,
		const std::string &fileName
		)
	{
		static const std::string Chars =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"1234567890";

		for (auto i = fileName.begin(); i != fileName.end(); ++i)
		{
			char c = *i;
			if (std::find(Chars.begin(), Chars.end(), c) == Chars.end())
			{
				c = '_';
			}
			else
			{
				c = std::toupper(c);
			}

			destination.put(c);
		}

		destination << "_INCLUDED_";

		std::random_device rng;
		std::uniform_int_distribution<> index_dist(0, Chars.size() - 1);

		static const size_t RandomPartLength = 16;

		for (size_t i = 0; i < RandomPartLength; ++i)
		{
			const char r = Chars[index_dist(rng)];
			destination.put(r);
		}
	}

	void printHelp()
	{
		std::cerr
			<< "Arguments:\n"
			<< "    header name (optional)\n"
			<< "    output (optional, default: stdout)\n"
			;
	}
}

int main(int argc, char **argv)
{
	std::ofstream file;
	std::ostream *destination;
	std::string headerName;

	if (argc == 1)
	{
		destination = &std::cout;
	}
	else if (argc == 2)
	{
		headerName = argv[1];
		destination = &std::cout;
	}
	else if (argc == 3)
	{
		headerName = argv[1];

		const std::string fileName = argv[2];
		file.open(fileName, std::ios::app);
		if (!file)
		{
			std::cerr << "Could not open destination file " << fileName << std::endl;
			return 1;
		}

		destination = &file;
	}
	else
	{
		printHelp();
		return 1;
	}

	generateGuardName(*destination, headerName);
}
