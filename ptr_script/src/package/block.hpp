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
		
		
		explicit block(statement_vector statements);
		const statement_vector &statements() const;
		
	private:
	
		statement_vector m_statements;
	};
}


#endif
