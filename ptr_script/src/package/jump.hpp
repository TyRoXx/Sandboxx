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
			std::size_t block_count
			);
		virtual void visit(statement_visitor &visitor) const PTR_SCRIPT_OVERRIDE;
		mode_t mode() const;
		std::size_t block_count() const;
		
	private:

		mode_t m_mode;
		std::size_t m_block_count;
	};
}


#endif
