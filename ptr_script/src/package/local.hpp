#ifndef LOCAL_HPP_INCLUDED_1C3CQGMCP8YVUH53
#define LOCAL_HPP_INCLUDED_1C3CQGMCP8YVUH53


#include "pointer.hpp"
#include <cstddef>


namespace ptrs
{
	struct ptr_type;


	struct local : pointer
	{
		explicit local(
			std::unique_ptr<ptr_type> type,
			std::size_t id
			);
		std::size_t id() const;

	private:

		std::size_t m_id;
	};
}


#endif
