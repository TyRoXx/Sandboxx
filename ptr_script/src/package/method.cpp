#include "method.hpp"


namespace ptrs
{
	method::method(
		std::string name,
		parameter_vector parameters,
		result_vector results,
		std::unique_ptr<statement> body
		)
		: m_name(std::move(name))
		, m_parameters(std::move(parameters))
		, m_results(std::move(results))
		, m_body(std::move(body))
	{
	}

	const std::string &method::name() const
	{
		return m_name;
	}

	const method::parameter_vector &method::parameters() const
	{
		return m_parameters;
	}

	const method::result_vector &method::results() const
	{
		return m_results;
	}

	const statement &method::body() const
	{
		return *m_body;
	}
}
