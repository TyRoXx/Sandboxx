#include "compiler.hpp"


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
		return intermediate::unit(
			m_unit_name
			);
	}
}
