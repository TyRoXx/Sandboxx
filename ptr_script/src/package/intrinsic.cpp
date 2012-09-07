#include "intrinsic.hpp"
#include "statement_visitor.hpp"


namespace ptrs
{
	void intrinsic::accept(statement_visitor &visitor) const
	{
		visitor.visit(*this);
	}
}
