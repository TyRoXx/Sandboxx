#include "unit.hpp"


namespace p0
{
	namespace intermediate
	{
		unit::unit(
			function_vector functions
			)
			: m_functions(std::move(functions))
		{
		}

		unit::function_vector const &unit::functions() const
		{
			return m_functions;
		}
	}
}
