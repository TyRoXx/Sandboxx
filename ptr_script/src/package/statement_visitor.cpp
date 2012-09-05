#include "statement_visitor.hpp"


namespace ptrs
{
	statement_visitor::~statement_visitor()
	{
	}
	
	void statement_visitor::accept(const block &statement)
	{
	}
	
	void statement_visitor::accept(const conditional &statement)
	{
	}
	
	void statement_visitor::accept(const jump &statement)
	{
	}
}
