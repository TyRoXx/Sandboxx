#include "function.hpp"


namespace p0
{
	namespace intermediate
	{
		function::function()
		{
		}

		function::function(
			instruction_vector body
			)
			: m_body(std::move(body))
		{
		}

		function::instruction_vector const &function::body() const
		{
			return m_body;
		}
	}
}
