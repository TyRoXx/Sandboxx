#ifndef LOCAL_HPP_INCLUDED_1C3CQGMCP8YVUH53
#define LOCAL_HPP_INCLUDED_1C3CQGMCP8YVUH53


#include "value.hpp"
#include <cstddef>


namespace ptrs
{
	struct local : value
	{
		explicit local(
			std::size_t id,
			const ptrs::type &type
			);
		std::size_t id() const;

	private:

		std::size_t m_id;
	};
}


#endif
