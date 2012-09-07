#include "value_visitor.hpp"


namespace ptrs
{
	value_visitor::~value_visitor()
	{
	}

	void value_visitor::visit(const local &value)
	{
	}

	void value_visitor::visit(const element_ptr &value)
	{
	}

	void value_visitor::visit(const literal &value)
	{
	}
}
