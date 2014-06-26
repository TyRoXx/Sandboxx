#ifndef NEW_LANG_AST_HPP
#define NEW_LANG_AST_HPP

#include "scanner.hpp"
#include <boost/variant.hpp>

namespace nl
{
	namespace ast
	{
		struct identifier
		{
			token position;
		};

		struct string
		{
			token position;
		};

		struct integer
		{
			token position;
		};

		struct lambda;
		struct subscript;
		struct call;

		typedef boost::variant<identifier, string, integer, boost::recursive_wrapper<lambda>, boost::recursive_wrapper<subscript>, boost::recursive_wrapper<call>> expression;

		struct definition;

		struct block
		{
			std::vector<definition> elements;
			expression result;
		};

		struct parameter;

		struct lambda
		{
			std::vector<parameter> parameters;
			boost::optional<expression> explicit_return_type;
			block body;
		};

		struct subscript
		{
			expression left;
			token element;
		};

		struct call
		{
			expression function;
			character_position argument_list;
			std::vector<expression> arguments;
		};

		struct parameter
		{
			expression type;
			token name;
		};

		struct definition
		{
			token name;
			expression value;
		};
	}
}

#endif
