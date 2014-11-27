#include <iostream>
#include <unordered_map>
#include <string>
#include <algorithm>

namespace
{
	std::unordered_map<std::string, std::function<void (std::string const &argument, std::ostream &)>> const command_translators
	{
		{"prints", [](std::string const &value, std::ostream &out)
			{
				out << "puts(" << value << ");\n";
			}
		},
		{"includes", [](std::string const &value, std::ostream &out)
			{
				out << "#include \"" << value << "\"\n";
			}
		},
		{"returns", [](std::string const &value, std::ostream &out)
			{
				out << "return " << value << ";\n";
			}
		},
		{"begin", [](std::string const &, std::ostream &out)
			{
				out << "{\n";
			}
		},
		{"end", [](std::string const &, std::ostream &out)
			{
				out << "}\n";
			}
		},
		{"main", [](std::string const &, std::ostream &out)
			{
				out << "int main(void)\n";
			}
		}
	};

	std::pair<std::string, std::string> split_command(std::string const &statement)
	{
		auto space = std::find(statement.begin(), statement.end(), ' ');
		std::string command(statement.begin(), space);
		if (space != statement.end())
		{
			return std::make_pair(std::move(command), std::string(space + 1, statement.end()));
		}
		return std::make_pair(std::move(command), std::string());
	}

	void translate_line(std::string const &full_line, std::ostream &translated)
	{
		auto first_slash = std::find(full_line.begin(), full_line.end(), '/');
		if (first_slash == full_line.end())
		{
			return;
		}
		auto second_slash = first_slash + 1;
		if (second_slash + 1 == full_line.end())
		{
			return;
		}
		if (*second_slash != '/')
		{
			return;
		}
		auto comment = std::string(second_slash + 1, full_line.end());
		translated << "/*" << comment << "*/\n";
		auto statement = split_command(comment);
		auto translator = command_translators.find(statement.first);
		if (translator != command_translators.end())
		{
			translator->second(statement.second, translated);
		}
	}
}

int main()
{
	std::string line;
	while (getline(std::cin, line))
	{
		translate_line(line, std::cout);
	}
}
