#include "element_ptr.hpp"
#include "value_visitor.hpp"


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

	void element_ptr::accept(value_visitor &visitor) const
	{
		visitor.visit(*this);
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
