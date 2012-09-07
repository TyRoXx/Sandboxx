#include "call.hpp"
#include "value_visitor.hpp"


namespace ptrs
{
	call::call(
		std::unique_ptr<value> method,
		argument_vector arguments,
		result_vector results
		)
		: m_method(std::move(method))
		, m_arguments(std::move(arguments))
		, m_results(std::move(results))
	{
	}

	void call::accept(value_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	const value &call::method() const
	{
		return *m_method;
	}

	const call::argument_vector &call::arguments() const
	{
		return m_arguments;
	}

	const call::result_vector &call::results() const
	{
		return m_results;
	}
}
