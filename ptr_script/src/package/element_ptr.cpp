#include "element_ptr.hpp"


namespace ptrs
{
	element_ptr::element_ptr(
		std::unique_ptr<value> object,
		std::size_t element_index
		)
		: m_object(std::move(object))
		, m_element_index(element_index)
	{
	}

	const value &element_ptr::object() const
	{
		return *m_object;
	}

	std::size_t element_ptr::element_index() const
	{
		return m_element_index;
	}
}
