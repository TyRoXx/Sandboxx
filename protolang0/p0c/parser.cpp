#include "parser.hpp"
#include "compiler_error.hpp"
#include "scanner.hpp"
#include "expression_tree.hpp"
#include "statement_tree.hpp"
#include <cassert>


namespace p0
{
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
		auto main_function = parse_function();
		skip_token(
			token_type::end_of_file,
			"End of file after main function expected"
			);
		return main_function;
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
					name_token.content,
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

	struct end_of_file_error : compiler_error
	{
		explicit end_of_file_error(
			std::string message,
			source_range position
			)
			: compiler_error(std::move(message), position)
		{
		}
	};

	std::unique_ptr<statement_tree> parser::parse_block()
	{
		block_tree::statement_vector body;

		//this block is only left with:
		//	break - on the closing '}'
		//	end_of_file_error - becase the block is incomplete in the input
		//	compiler_error - when the error handling function returns false
		for (;;)
		{
			try
			{
				if (peek_token().type == token_type::end_of_file)
				{
					throw end_of_file_error(
						"Unexpected end of file in block",
						m_scanner.next_token().content
						);
				}

				//end of the block
				if (try_skip_token(
					token_type::brace_right
					))
				{
					break;
				}

				auto statement = parse_statement();
				body.push_back(
					std::move(statement)
					);
			}
			catch (end_of_file_error const &)
			{
				//this error is best handled at the top-most level
				throw;
			}
			catch (compiler_error const &e)
			{
				if (!m_error_handler(e))
				{
					throw;
				}

				//Try again at the next line to find more potential errors.
				//The next statement does not necessarily start on the very next
				//line, but this is a good approximation for real world sources.
				skip_line();
			}
		}

		return std::unique_ptr<statement_tree>(new block_tree(
			std::move(body)
			));
	}

	std::unique_ptr<expression_tree> parser::parse_expression()
	{
		auto left = parse_primary_expression();

		//postfix call syntax like in C++
		//f(a, b, c)
		//left = f
		//arguments = {a, b, c}

		while (try_skip_token(
			token_type::parenthesis_left
			))
		{
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

			std::unique_ptr<expression_tree> call(new call_expression_tree(
				std::move(left),
				std::move(arguments)
				));

			left = std::move(call);
		}

		return std::move(left);
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
				//any expression can be enclosed in parentheses
				auto value = parse_expression();
				auto const closing_parenthesis = pop_token();
				expect_token_type(
					closing_parenthesis,
					token_type::parenthesis_right,
					"Closing parenthesis ')' expected"
					);
				return std::move(value);
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

	void parser::expect_token_type(token const &token, token_type::Enum type, std::string const &message) const
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

	void parser::skip_line()
	{
		m_scanner.skip_line();
		m_is_next_token = false;
	}
}
