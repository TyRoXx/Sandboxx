#ifndef PACKAGE_INFO_HPP_INCLUDED_UIQ8B1LLHC9UAB0W
#define PACKAGE_INFO_HPP_INCLUDED_UIQ8B1LLHC9UAB0W


#include "common/guid.hpp"
#include <string>
#include <set>


namespace ptrs
{
	typedef std::set<guid> dependency_set;


	struct package_info
	{
		explicit package_info(
			const guid &id,
			std::string title,
			dependency_set dependencies
			);
		const guid &id() const;
		const std::string &title() const;
		const dependency_set &dependencies() const;

	private:

		guid m_id;
		std::string m_title;
		dependency_set m_dependencies;
	};
}


#endif
