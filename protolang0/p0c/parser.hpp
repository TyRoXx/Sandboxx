#pragma once
#ifndef P0C_PARSER_HPP
#define P0C_PARSER_HPP


#include "function_tree.hpp"
#include "token.hpp"


namespace p0
{
	struct scanner;


	struct parser
	{
		explicit parser(
			scanner &scanner
			);
		function_tree parse_unit();

	private:

		scanner &m_scanner;


		function_tree parse_function();
		std::unique_ptr<statement_tree> parse_statement();
		std::unique_ptr<expression_tree> parse_expression();
		void expect_token_type(const token &token, token_type::Enum type, const std::string &message) const;
	};
}


#endif
