#ifndef STATEMENT_HPP_INCLUDED_R56M05EYGK4QBER6
#define STATEMENT_HPP_INCLUDED_R56M05EYGK4QBER6


#include "common/override.hpp"


namespace ptrs
{
	struct statement_visitor;
	
	
	struct statement
	{
		virtual ~statement();
		virtual void visit(statement_visitor &visitor) const = 0;
	};
}


#endif
