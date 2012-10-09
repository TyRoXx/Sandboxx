#include "compiler.hpp"
#include "scanner.hpp"
#include "parser.hpp"
#include "code_generator.hpp"


namespace p0
{
	compiler::compiler(
		std::string unit_name,
		source_range source
		)
		: m_unit_name(std::move(unit_name))
		, m_source(source)
	{
	}

	intermediate::unit compiler::compile()
	{
		scanner scanner(m_source);
		parser parser(scanner);

		auto const tree = parser.parse_unit();

		code_generator generator(tree);

		return generator.generate_unit();
	}
}
