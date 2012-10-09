#pragma once
#ifndef P0C_PARSER_HPP
#define P0C_PARSER_HPP


#include "token.hpp"
#include <functional>


namespace p0
{
	struct scanner;
	struct compiler_error;
	struct statement_tree;
	struct expression_tree;


	struct parser
	{
		typedef std::function<bool (const compiler_error &)> compiler_error_handler;


		explicit parser(
			scanner &scanner,
			compiler_error_handler error_handler
			);
		std::unique_ptr<expression_tree> parse_unit();

	private:

		scanner &m_scanner;
		compiler_error_handler const m_error_handler;
		token m_next_token;
		bool m_is_next_token;


		std::unique_ptr<statement_tree> parse_statement();
		std::unique_ptr<statement_tree> parse_block();
		std::unique_ptr<expression_tree> parse_expression();
		std::unique_ptr<expression_tree> parse_function();
		void expect_token_type(const token &token, token_type::Enum type, const std::string &message) const;
		token const &peek_token();
		token pop_token();
		bool try_skip_token(token_type::Enum type);
		void skip_token(token_type::Enum type, char const *message);
	};
}


#endif
