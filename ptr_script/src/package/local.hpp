#ifndef LOCAL_HPP_INCLUDED_1C3CQGMCP8YVUH53
#define LOCAL_HPP_INCLUDED_1C3CQGMCP8YVUH53


#include "value.hpp"
#include <cstddef>


namespace ptrs
{
	struct ptr_type;


	struct local : value
	{
		explicit local(
			std::size_t id
			);
		virtual void accept(value_visitor &visitor) const PTR_SCRIPT_OVERRIDE;
		std::size_t id() const;

	private:

		std::size_t m_id;
	};
}


#endif
