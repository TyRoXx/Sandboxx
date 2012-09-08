#include "execute_statement.hpp"
#include "package/statement_visitor.hpp"
#include "package/conditional.hpp"
#include "package/block.hpp"
#include "package/jump.hpp"
#include "package/call.hpp"
#include "package/intrinsic.hpp"


namespace ptrs
{
	namespace
	{
		struct statement_executor : statement_visitor
		{
			explicit statement_executor(interpreter &interpreter)
				: m_interpreter(interpreter)
			{
			}

			virtual void visit(const block &statement) PTR_SCRIPT_OVERRIDE
			{

			}

			virtual void visit(const conditional &statement) PTR_SCRIPT_OVERRIDE
			{

			}

			virtual void visit(const jump &statement) PTR_SCRIPT_OVERRIDE
			{

			}

			virtual void visit(const call_statement &statement) PTR_SCRIPT_OVERRIDE
			{

			}

			virtual void visit(const intrinsic &statement) PTR_SCRIPT_OVERRIDE
			{

			}

		private:

			interpreter &m_interpreter;
		};
	}


	void execute_statement(
		interpreter &interpreter,
		const statement &statement
		)
	{
		statement_executor executor(interpreter);
		statement.accept(executor);
	}
}
