#ifndef INTERPRETER_HPP_INCLUDED_
#define INTERPRETER_HPP_INCLUDED_


#include "common/guid.hpp"
#include <map>


namespace ptrs
{
	struct package;


	struct interpreter
	{
		typedef std::map<guid, const package *> package_by_id;


		explicit interpreter(package_by_id packages);
		const package_by_id &packages() const;

	private:

		package_by_id m_packages;
	};
}


#endif
