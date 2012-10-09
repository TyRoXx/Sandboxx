#include "function.hpp"


namespace p0
{
	namespace intermediate
	{
		function::function()
			: m_parameters(0)
			, m_locals(0)
		{
		}

		function::function(
			instruction_vector body,
			size_t parameters,
			size_t locals
			)
			: m_body(std::move(body))
			, m_parameters(parameters)
			, m_locals(locals)
		{
		}

		function::instruction_vector const &function::body() const
		{
			return m_body;
		}

		size_t function::parameters() const
		{
			return m_parameters;
		}

		size_t function::locals() const
		{
			return m_locals;
		}
	}
}
