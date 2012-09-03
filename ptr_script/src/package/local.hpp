#ifndef LOCAL_HPP_INCLUDED_1C3CQGMCP8YVUH53
#define LOCAL_HPP_INCLUDED_1C3CQGMCP8YVUH53


#include "value.hpp"
#include "common/override.hpp"


namespace ptrs
{
	struct local : value
	{
		explicit local(const ptrs::type &type);
		virtual void print(std::ostream &os) const PTR_SCRIPT_OVERRIDE;

	private:
	};
}


#endif
