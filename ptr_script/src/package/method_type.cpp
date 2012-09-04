#include "method_type.hpp"


namespace ptrs
{
	method_type::method_type(
		const structure_ref &structure,
		type_vector parameters,
		type_vector results
		)
		: m_structure(structure)
		, m_parameters(std::move(parameters))
		, m_results(std::move(parameters))
	{
	}

	const structure_ref &method_type::structure() const
	{
		return m_structure;
	}

	const method_type::type_vector &method_type::parameters() const
	{
		return m_parameters;
	}

	const method_type::type_vector &method_type::results() const
	{
		return m_results;
	}
}
