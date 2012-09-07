#ifndef PRINT_PACKAGE_HPP_INCLUDED_
#define PRINT_PACKAGE_HPP_INCLUDED_


#include <ostream>


namespace ptrs
{
	struct package;
	struct method;
	struct structure;
	struct element;
	struct type;
	struct value;
	struct statement;


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

	void print_element(
		std::ostream &os,
		const element &element
		);

	void print_type(
		std::ostream &os,
		const type &type
		);

	void print_value(
		std::ostream &os,
		const value &value
		);

	void print_statement(
		std::ostream &os,
		const statement &statement
		);
}


#endif
