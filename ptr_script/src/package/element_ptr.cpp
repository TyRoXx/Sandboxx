#include "element_ptr.hpp"
#include "ptr_type.hpp"
#include "pointer.hpp"


namespace ptrs
{
	element_ptr::element_ptr(
		std::unique_ptr<pointer> object,
		std::size_t element_index
		)
		: m_object(std::move(object))
		, m_element_index(element_index)
	{
	}

	const pointer &element_ptr::object() const
	{
		return *m_object;
	}

	std::size_t element_ptr::element_index() const
	{
		return m_element_index;
	}
}
