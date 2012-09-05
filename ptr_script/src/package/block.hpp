#ifndef BLOCK_HPP_INCLUDED_5DUHDYFRTCO02Y9E
#define BLOCK_HPP_INCLUDED_5DUHDYFRTCO02Y9E


#include "statement.hpp"
#include <vector>
#include <memory>


namespace ptrs
{
	struct block : statement
	{
		typedef std::vector<std::unique_ptr<statement>> statement_vector;
		
		
		explicit block(
			statement_vector statements,
			bool is_jump_target
			);
		virtual void visit(statement_visitor &visitor) const PTR_SCRIPT_OVERRIDE;
		const statement_vector &statements() const;
		bool is_jump_target() const;
		
	private:
	
		statement_vector m_statements;
		bool m_is_jump_target;
	};
}


#endif
