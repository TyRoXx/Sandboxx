#ifndef INTRINSIC_HPP_INCLUDED_
#define INTRINSIC_HPP_INCLUDED_


#include "statement.hpp"


namespace ptrs
{
	struct intrinsic : statement
	{
		virtual void accept(statement_visitor &visitor) const PTR_SCRIPT_OVERRIDE;
		
	private:
	};
}


#endif
