#include "call_statement.hpp"
#include "statement_visitor.hpp"


namespace ptrs
{
	call_statement::call_statement(
		std::unique_ptr<ptrs::call> call
		)
		: m_call(std::move(call))
	{
	}

	void call_statement::accept(statement_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	const call &call_statement::call() const
	{
		return *m_call;
	}
}
