#include "interpreter.hpp"
#include "package/package.hpp"
#include "package/statement_visitor.hpp"


namespace ptrs
{
	interpreter::interpreter(
		package_by_id packages
		)
		: m_packages(std::move(packages))
	{
	}

	void interpreter::call(const method &callee)
	{
		executeStatement(callee.body());
	}


	void interpreter::executeStatement(const statement &statement)
	{

	}
}
