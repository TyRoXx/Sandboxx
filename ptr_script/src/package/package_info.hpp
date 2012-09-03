#ifndef PACKAGE_INFO_HPP_INCLUDED_UIQ8B1LLHC9UAB0W
#define PACKAGE_INFO_HPP_INCLUDED_UIQ8B1LLHC9UAB0W


#include "common/guid.hpp"
#include <string>
#include <set>
#include <istream>
#include <ostream>


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
		package_info();
		const guid &id() const;
		const std::string &title() const;
		const dependency_set &dependencies() const;

	private:

		guid m_id;
		std::string m_title;
		dependency_set m_dependencies;
	};


	bool operator == (const package_info &left, const package_info &right);


	std::ostream &operator << (std::ostream &os, const package_info &p);
	std::istream &operator >> (std::istream &is, package_info &p);
}


#endif
