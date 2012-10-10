#include "parser.hpp"
#include "compiler_error.hpp"
#include "scanner.hpp"
#include "expression_tree.hpp"
#include "statement_tree.hpp"
#include <cassert>


namespace p0
{
	namespace
	{
		std::string source_range_to_string(source_range source)
		{
			return std::string(
				source.begin(),
				source.end()
				);
		}
	}


	parser::parser(
		scanner &scanner,
		compiler_error_handler error_handler
		)
		: m_scanner(scanner)
		, m_error_handler(std::move(error_handler))
		, m_is_next_token(false)
	{
		assert(m_error_handler);
	}

	std::unique_ptr<function_tree> parser::parse_unit()
	{
		return parse_function();
	}


	std::unique_ptr<statement_tree> parser::parse_statement()
	{
		token const first = peek_token();
		switch (first.type)
		{
		case token_type::var:
			{
				pop_token();
				token const name_token = pop_token();
				expect_token_type(
					name_token,
					token_type::identifier,
					"Name of declared variable expected"
					);

				skip_token(
					token_type::assign,
					"Assignment operator '=' expected"
					);

				auto value = parse_expression();
				return std::unique_ptr<statement_tree>(new declaration_tree(
					source_range_to_string(name_token.content),
					std::move(value)
					));
			}

		case token_type::return_:
			{
				pop_token();
				auto value = parse_expression();
				return std::unique_ptr<statement_tree>(new return_tree(
					std::move(value)
					));
			}

		case token_type::brace_left:
			{
				pop_token();
				auto body = parse_block();
				return std::move(body);
			}

		default:
			{
				auto left = parse_expression();

				if (try_skip_token(token_type::assign))
				{
					auto right = parse_expression();

					return std::unique_ptr<statement_tree>(new assignment_tree(
						std::move(left),
						std::move(right)
						));
				}
				else
				{
					return std::unique_ptr<statement_tree>(new expression_statement_tree(
						std::move(left)
						));
				}
			}
		}
	}

	std::unique_ptr<statement_tree> parser::parse_block()
	{
		block_tree::statement_vector body;

		while (!try_skip_token(
			token_type::brace_right
			))
		{
			try
			{
				auto statement = parse_statement();
				body.push_back(
					std::move(statement)
					);
			}
			catch (compiler_error const &e)
			{
				if (!m_error_handler(e))
				{
					throw;
				}

				m_scanner.skip_line();
			}
		}

		return std::unique_ptr<statement_tree>(new block_tree(
			std::move(body)
			));
	}

	std::unique_ptr<expression_tree> parser::parse_expression()
	{
		return parse_primary_expression();
	}

	std::unique_ptr<expression_tree> parser::parse_primary_expression()
	{
		auto const first = pop_token();
		switch (first.type)
		{
		case token_type::identifier:
			return std::unique_ptr<expression_tree>(
				new name_expression_tree(
					first.content
				));

		case token_type::integer_10:
			return std::unique_ptr<expression_tree>(
				new integer_10_expression_tree(
					first.content
				));

		case token_type::parenthesis_left:
			{
				auto value = parse_expression();
				auto const closing_parenthesis = pop_token();
				expect_token_type(
					closing_parenthesis,
					token_type::parenthesis_right,
					"Closing parenthesis ')' expected"
					);
				return std::move(value);
			}

		case token_type::call:
			{
				auto function = parse_expression();

				skip_token(
					token_type::parenthesis_left,
					"Opening parenthesis '(' expected"
					);

				call_expression_tree::expression_vector arguments;

				for (;;)
				{
					if (try_skip_token(token_type::parenthesis_right))
					{
						break;
					}

					auto argument = parse_expression();
					arguments.push_back(
						std::move(argument)
						);

					if (!try_skip_token(token_type::comma))
					{
						skip_token(
							token_type::parenthesis_right,
							"Comma or closing parenthesis expected"
							);
						break;
					}
				}

				return std::unique_ptr<expression_tree>(new call_expression_tree(
					std::move(function),
					std::move(arguments)
					));
			}

		case token_type::function:
			{
				return parse_function();
			}

		case token_type::null:
			{
				return std::unique_ptr<expression_tree>(
					new null_expression_tree
					);
			}

		default:
			throw compiler_error(
				"Expression expected",
				first.content
				);
		}
	}

	std::unique_ptr<function_tree> parser::parse_function()
	{
		function_tree::name_vector parameters;

		//the parameter list is optional
		if (try_skip_token(
			token_type::parenthesis_left
			))
		{
			for (;;)
			{
				auto const name = peek_token();
				if (name.type == token_type::identifier)
				{
					pop_token();
					parameters.push_back(name.content);

					if (!try_skip_token(
						token_type::comma
						))
					{
						break;
					}
				}
				else
				{
					break;
				}
			}

			skip_token(
				token_type::parenthesis_right,
				"Closing parenthesis ')' after parameter list expected"
				);
		}

		skip_token(
			token_type::brace_left,
			"Opening brace '{' of function body expected"
			);

		auto body = parse_block();
		return std::unique_ptr<function_tree>(new function_tree(
			std::move(body),
			std::move(parameters)
			));
	}

	void parser::expect_token_type(const token &token, token_type::Enum type, const std::string &message) const
	{
		if (token.type != type)
		{
			throw compiler_error(
				message,
				token.content
				);
		}
	}

	token const &parser::peek_token()
	{
		if (!m_is_next_token)
		{
			m_next_token = m_scanner.next_token();
			m_is_next_token = true;
		}

		return m_next_token;
	}

	token parser::pop_token()
	{
		if (m_is_next_token)
		{
			m_is_next_token = false;
			return m_next_token;
		}
		else
		{
			return m_scanner.next_token();
		}
	}

	bool parser::try_skip_token(token_type::Enum type)
	{
		auto const token = peek_token();
		if (token.type == type)
		{
			pop_token();
			return true;
		}

		return false;
	}

	void parser::skip_token(token_type::Enum type, char const *message)
	{
		if (!try_skip_token(type))
		{
			throw compiler_error(
				message,
				peek_token().content
				);
		}
	}
}
