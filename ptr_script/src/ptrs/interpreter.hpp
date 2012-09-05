#ifndef INTERPRETER_HPP_INCLUDED_
#define INTERPRETER_HPP_INCLUDED_


#include "common/guid.hpp"
#include <map>


namespace ptrs
{
	struct package;
	struct statement;
	struct method;


	struct interpreter
	{
		typedef std::map<guid, const package *> package_by_id;


		explicit interpreter(
			package_by_id packages
			);
		void call(const method &callee);

	private:

		package_by_id m_packages;


		void executeStatement(const statement &statement);
	};
}


#endif
