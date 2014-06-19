#ifndef NEW_LANG_PARSER_HPP
#define NEW_LANG_PARSER_HPP

#include "ast.hpp"
#include "scanner.hpp"

namespace nl
{
	namespace ast
	{
		struct parser_error : std::runtime_error
		{
			explicit parser_error(std::string message)
				: std::runtime_error(std::move(message))
			{
			}
		};

		inline void expect_token(Si::source<token> &tokens, token_type expected)
		{
			auto found = peek(tokens);
			if (!found ||
				(found->type != expected))
			{
				throw parser_error(std::string("Expected token ") + get_token_name(expected));
			}
			Si::get(tokens);
		}

		inline void expect_indentation(Si::source<token> &tokens, std::size_t indentation)
		{
			for (size_t i = 0; i < indentation; ++i)
			{
				expect_token(tokens, token_type::tab);
			}
		}

		inline bool is_next_token(Si::source<token> &tokens, token_type expected)
		{
			auto &&next = peek(tokens);
			return (next && (next->type == expected));
		}

		expression parse_expression(Si::source<token> &tokens, std::size_t indentation);

		inline definition parse_definition(Si::source<token> &tokens, std::size_t indentation)
		{
			auto const next = peek(tokens);
			if (!next)
			{
				throw parser_error("definition expected");
			}
			switch (next->type)
			{
			case token_type::identifier:
				{
					auto name = Si::get(tokens);
					expect_token(tokens, token_type::space);
					expect_token(tokens, token_type::assignment);
					expect_token(tokens, token_type::space);
					auto value = parse_expression(tokens, indentation);
					expect_token(tokens, token_type::newline);
					return definition{std::move(*name), std::move(value)};
				}

			default:
				throw parser_error("identifier for definition name expected");
			}
		}

		inline token expect_identifier(Si::source<token> &tokens)
		{
			auto found = peek(tokens);
			if (!found ||
				(found->type != token_type::identifier))
			{
				throw parser_error("identifier expected");
			}
			return std::move(*Si::get(tokens));
		}

		block parse_block(Si::source<token> &tokens, std::size_t indentation);

		inline parameter parse_parameter(Si::source<token> &tokens, std::size_t indentation)
		{
			auto type = parse_expression(tokens, indentation);
			expect_token(tokens, token_type::space);
			auto name = expect_identifier(tokens);
			return parameter{std::move(type), std::move(name)};
		}

		inline std::vector<parameter> parse_parameters(Si::source<token> &tokens, std::size_t indentation)
		{
			std::vector<parameter> parameters;
			for (;;)
			{
				auto closing_parenthesis = peek(tokens);
				if (!closing_parenthesis)
				{
					throw parser_error("premature end of tokens in parameter list");
				}
				if (closing_parenthesis->type == token_type::right_parenthesis)
				{
					Si::get(tokens);
					break;
				}
				if (!parameters.empty())
				{
					expect_token(tokens, token_type::comma);
					expect_token(tokens, token_type::space);
				}
				parameters.emplace_back(parse_parameter(tokens, indentation));
			}
			return parameters;
		}

		inline expression parse_left_expression(Si::source<token> &tokens, std::size_t indentation)
		{
			auto const next = peek(tokens);
			if (!next)
			{
				throw parser_error("premature end of tokens where an expression is expected");
			}
			switch (next->type)
			{
			case token_type::identifier:
				{
					return identifier{*Si::get(tokens)};
				}

			case token_type::integer:
				{
					return integer{*Si::get(tokens)};
				}

			case token_type::string:
				{
					return string{*Si::get(tokens)};
				}

			case token_type::left_parenthesis:
				{
					Si::get(tokens);
					auto parameters = parse_parameters(tokens, indentation);
					boost::optional<expression> explicit_return_type;
					if (is_next_token(tokens, token_type::space))
					{
						//pop the space
						Si::get(tokens);
						explicit_return_type = parse_expression(tokens, indentation);
					}
					expect_token(tokens, token_type::newline);
					auto body = parse_block(tokens, indentation + 1);
					return lambda{std::move(parameters), std::move(explicit_return_type), std::move(body)};
				}

			default:
				throw parser_error("expected an expression");
			}
		}

		inline expression parse_expression(Si::source<token> &tokens, std::size_t indentation)
		{
			auto left = parse_left_expression(tokens, indentation);
			for (;;)
			{
				auto next = peek(tokens);
				if (!next)
				{
					return left;
				}
				switch (next->type)
				{
				case token_type::dot:
					{
						Si::get(tokens);
						auto element = expect_identifier(tokens);
						left = subscript{std::move(left), std::move(element)};
						break;
					}

				case token_type::left_parenthesis:
					{
						Si::get(tokens);
						std::vector<expression> arguments;
						for (;;)
						{
							auto closing_parenthesis = peek(tokens);
							if (closing_parenthesis &&
								closing_parenthesis->type == token_type::right_parenthesis)
							{
								Si::get(tokens);
								break;
							}
							if (!arguments.empty())
							{
								expect_token(tokens, token_type::comma);
								expect_token(tokens, token_type::space);
							}
							arguments.emplace_back(parse_expression(tokens, indentation));
						}
						left = call{std::move(left), std::move(arguments)};
						break;
					}

				default:
					return left;
				}
			}
		}

		inline block parse_block(Si::source<token> &tokens, std::size_t indentation)
		{
			std::vector<definition> elements;
			for (;;)
			{
				{
					auto next = peek(tokens);
					if (next &&
						next->type == token_type::newline)
					{
						Si::get(tokens);
						continue;
					}
				}

				expect_indentation(tokens, indentation);

				auto next = peek(tokens);
				if (next &&
					next->type == token_type::return_)
				{
					Si::get(tokens);
					expect_token(tokens, token_type::space);
					auto result = parse_expression(tokens, indentation);
					return block{std::move(elements), std::move(result)};
				}

				elements.emplace_back(parse_definition(tokens, indentation));
			}
		}
	}
}

#endif
