#ifndef CALL_STATEMENT_HPP_INCLUDED_
#define CALL_STATEMENT_HPP_INCLUDED_


#include "statement.hpp"
#include "call.hpp"
#include <memory>


namespace ptrs
{
	struct call_statement : statement
	{
		explicit call_statement(
			std::unique_ptr<call> call
			);
		virtual void accept(statement_visitor &visitor) const PTR_SCRIPT_OVERRIDE;
		const call &call() const;
		
	private:
	
		std::unique_ptr<ptrs::call> m_call;
	};
}


#endif
