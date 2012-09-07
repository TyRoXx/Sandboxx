#ifndef CONDITIONAL_HPP_INCLUDED_MZUYRK7QCKFI34W0
#define CONDITIONAL_HPP_INCLUDED_MZUYRK7QCKFI34W0


#include "statement.hpp"
#include <memory>
#include <vector>
#include <utility>


namespace ptrs
{
	struct value;
	
	
	struct conditional : statement
	{
		typedef std::pair<
			std::unique_ptr<value>,
			std::unique_ptr<statement>
			> branch;
		typedef std::vector<branch> branch_vector;
		
		
		explicit conditional(
			branch_vector branches,
			std::unique_ptr<statement> otherwise
			);
		virtual void accept(statement_visitor &visitor) const PTR_SCRIPT_OVERRIDE;
		const branch_vector &branches() const;
		const statement *otherwise() const;
		
	private:
	
		branch_vector m_branches;
		std::unique_ptr<statement> m_otherwise;
	};
}


#endif
