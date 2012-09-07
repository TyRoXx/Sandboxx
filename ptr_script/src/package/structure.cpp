#include "structure.hpp"


namespace ptrs
{
	structure::structure(
		std::string full_name,
		method_vector methods,
		element_vector elements
		)
		: m_full_name(std::move(full_name))
		, m_methods(std::move(methods))
		, m_elements(std::move(elements))
	{
	}

	const std::string &structure::full_name() const
	{
		return m_full_name;
	}

	const structure::method_vector &structure::methods() const
	{
		return m_methods;
	}

	const structure::element_vector &structure::elements() const
	{
		return m_elements;
	}
}
