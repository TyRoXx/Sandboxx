#include "save_unit.hpp"
#include "unit.hpp"
#include "function.hpp"
#include <algorithm>
#include <iomanip>


namespace p0
{
	namespace intermediate
	{
		namespace
		{
			void save_instruction(
				std::ostream &file,
				instruction const &instruction
				)
			{
				using namespace std;

				auto const &info = get_instruction_info(instruction.type());

				file << "  ";
				file << std::setw(18) << std::left << info.name;

				std::for_each(
					begin(instruction.arguments()),
					begin(instruction.arguments()) + info.argument_count,
					[&file](instruction_argument arg)
				{
					file << ' ' << std::setw(6) << arg;
				});

				file << '\n';
			}

			void save_function(
				std::ostream &file,
				function const &function
				)
			{
				file << "function\n";
				for (auto i = begin(function.body()); i != end(function.body()); ++i)
				{
					save_instruction(file, *i);
				}
				file << "end\n";
			}
		}


		void save_unit(
			std::ostream &file,
			unit const &unit
			)
		{
			for (auto f = begin(unit.functions()); f != end(unit.functions()); ++f)
			{
				save_function(file, *f);

				file << '\n';
			}

			for (size_t s = 0; s < unit.strings().size(); ++s)
			{
				file
					<< std::setw(3)
					<< std::setfill('0')
					<< std::right
					<< s << ": "
					
					<< unit.strings()[s]
					<< '\n';
			}
		}
	}
}
