#ifndef PRIMITIVE_TYPE_HPP_INCLUDED_42YE82LM7B2P0L7E
#define PRIMITIVE_TYPE_HPP_INCLUDED_42YE82LM7B2P0L7E


#include "type.hpp"
#include "common/override.hpp"


namespace ptrs
{
	struct primitive_type : type
	{
		explicit primitive_type();
		virtual void print_name(std::ostream &os) const PTR_SCRIPT_OVERRIDE;
		
	private:
	};
}


#endif
