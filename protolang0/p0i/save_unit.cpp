#include "save_unit.hpp"
#include "unit.hpp"
#include "function.hpp"


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

				file << instruction.type();
				std::for_each(
					begin(instruction.arguments()),
					end(instruction.arguments()),
					[&file](instruction_argument arg)
				{
					file << ' ' << arg;
				});
				file << '\n';
			}

			void save_function(
				std::ostream &file,
				function const &function
				)
			{
				for (auto i = begin(function.body()); i != end(function.body()); ++i)
				{
					save_instruction(file, *i);
				}
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
		}
	}
}
