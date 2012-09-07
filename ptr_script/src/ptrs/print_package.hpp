#ifndef PRINT_PACKAGE_HPP_INCLUDED_
#define PRINT_PACKAGE_HPP_INCLUDED_


#include <ostream>


namespace ptrs
{
	struct package;
	struct method;
	struct structure;
	struct type;
	struct value;


	void print_package(
		std::ostream &os,
		const package &package
		);

	void print_method(
		std::ostream &os,
		const method &method
		);

	void print_structure(
		std::ostream &os,
		const structure &structure
		);

	void print_type(
		std::ostream &os,
		const type &type
		);

	void print_value(
		std::ostream &os,
		const value &value
		);
}


#endif
