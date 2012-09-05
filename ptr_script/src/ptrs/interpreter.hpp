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


		explicit interpreter(
			package_by_id packages
			);
		void call(std::size_t free_method_id);

	private:

		package_by_id m_packages;
	};
}


#endif
