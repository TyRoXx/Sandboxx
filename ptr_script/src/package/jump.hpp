#ifndef JUMP_HPP_INCLUDED_7SK0L2BRNPEZ2K2L
#define JUMP_HPP_INCLUDED_7SK0L2BRNPEZ2K2L


#include "statement.hpp"
#include <cstddef>


namespace ptrs
{
	struct jump : statement
	{
		enum mode_t
		{
			break_,
			continue_,
		};
		
		
		explicit jump(
			mode_t mode,
			std::size_t scope_count
			);
		mode_t mode() const;
		std::size_t scope_count() const;
		
	private:

		mode_t m_mode;
		std::size_t m_scope_count;
	};
}


#endif
