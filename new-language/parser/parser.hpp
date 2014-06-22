#ifndef NEW_LANG_PARSER_HPP
#define NEW_LANG_PARSER_HPP

#include "ast.hpp"
#include "scanner.hpp"
#include <boost/format.hpp>

namespace nl
{
	namespace ast
	{
		struct parser
		{
			parser(Si::source<token> &tokens)
				: tokens(tokens)
			{
			}

			boost::optional<token> peek()
			{
				return nl::peek(tokens);
			}

			boost::optional<token> get()
			{
				auto got = Si::get(tokens);
				if (got)
				{
					last_token_position_ = got->begin;
				}
				return got;
			}

			character_position last_token_position() const
			{
				return last_token_position_;
			}

		private:

			Si::source<token> &tokens;
			character_position last_token_position_;
		};

		struct parser_error : std::runtime_error
		{
			explicit parser_error(std::string message)
				: std::runtime_error(std::move(message))
			{
			}
		};

		parser_error make_parser_error(std::string const &message, character_position where)
		{
			return parser_error(message + " (" + boost::str(boost::format("%1%:%2%") % where.line % where.column) + ")");
		}

		inline void expect_token(parser &tokens, token_type expected)
		{
			auto found = tokens.peek();
			if (!found ||
				(found->type != expected))
			{
				throw make_parser_error(std::string("Expected token ") + get_token_name(expected), tokens.last_token_position());
			}
			tokens.get();
		}

		inline void expect_indentation(parser &tokens, std::size_t indentation)
		{
			for (size_t i = 0; i < indentation; ++i)
			{
				expect_token(tokens, token_type::tab);
			}
		}

		inline bool is_next_token(parser &tokens, token_type expected)
		{
			auto &&next = tokens.peek();
			return (next && (next->type == expected));
		}

		expression parse_expression(parser &tokens, std::size_t indentation);

		inline definition parse_definition(parser &tokens, std::size_t indentation)
		{
			auto const next = tokens.peek();
			if (!next)
			{
				throw make_parser_error("definition expected", tokens.last_token_position());
			}
			switch (next->type)
			{
			case token_type::identifier:
				{
					auto name = tokens.get();
					expect_token(tokens, token_type::space);
					expect_token(tokens, token_type::assignment);
					expect_token(tokens, token_type::space);
					auto value = parse_expression(tokens, indentation);
					expect_token(tokens, token_type::newline);
					return definition{std::move(*name), std::move(value)};
				}

			default:
				throw make_parser_error("identifier for definition name expected", tokens.last_token_position());
			}
		}

		inline token expect_identifier(parser &tokens)
		{
			auto found = tokens.peek();
			if (!found ||
				(found->type != token_type::identifier))
			{
				throw make_parser_error("identifier expected", tokens.last_token_position());
			}
			return std::move(*tokens.get());
		}

		block parse_block(parser &tokens, std::size_t indentation);

		inline parameter parse_parameter(parser &tokens, std::size_t indentation)
		{
			auto type = parse_expression(tokens, indentation);
			expect_token(tokens, token_type::space);
			auto name = expect_identifier(tokens);
			return parameter{std::move(type), std::move(name)};
		}

		inline std::vector<parameter> parse_parameters(parser &tokens, std::size_t indentation)
		{
			std::vector<parameter> parameters;
			for (;;)
			{
				auto closing_parenthesis = tokens.peek();
				if (!closing_parenthesis)
				{
					throw make_parser_error("premature end of tokens in parameter list", tokens.last_token_position());
				}
				if (closing_parenthesis->type == token_type::right_parenthesis)
				{
					tokens.get();
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

		inline expression parse_left_expression(parser &tokens, std::size_t indentation)
		{
			auto const next = tokens.peek();
			if (!next)
			{
				throw make_parser_error("premature end of tokens where an expression is expected", tokens.last_token_position());
			}
			switch (next->type)
			{
			case token_type::identifier:
				{
					return identifier{*tokens.get()};
				}

			case token_type::integer:
				{
					return integer{*tokens.get()};
				}

			case token_type::string:
				{
					return string{*tokens.get()};
				}

			case token_type::left_parenthesis:
				{
					tokens.get();
					auto parameters = parse_parameters(tokens, indentation);
					boost::optional<expression> explicit_return_type;
					if (is_next_token(tokens, token_type::space))
					{
						//pop the space
						tokens.get();
						explicit_return_type = parse_expression(tokens, indentation);
					}
					expect_token(tokens, token_type::newline);
					auto body = parse_block(tokens, indentation + 1);
					return lambda{std::move(parameters), std::move(explicit_return_type), std::move(body)};
				}

			default:
				throw make_parser_error("expected an expression", tokens.last_token_position());
			}
		}

		inline expression parse_expression(parser &tokens, std::size_t indentation)
		{
			auto left = parse_left_expression(tokens, indentation);
			for (;;)
			{
				auto next = tokens.peek();
				if (!next)
				{
					return left;
				}
				switch (next->type)
				{
				case token_type::dot:
					{
						tokens.get();
						auto element = expect_identifier(tokens);
						left = subscript{std::move(left), std::move(element)};
						break;
					}

				case token_type::left_parenthesis:
					{
						tokens.get();
						std::vector<expression> arguments;
						for (;;)
						{
							auto closing_parenthesis = tokens.peek();
							if (closing_parenthesis &&
								closing_parenthesis->type == token_type::right_parenthesis)
							{
								tokens.get();
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

		inline block parse_block(parser &tokens, std::size_t indentation)
		{
			std::vector<definition> elements;
			for (;;)
			{
				{
					auto next = tokens.peek();
					if (next &&
						next->type == token_type::newline)
					{
						tokens.get();
						continue;
					}
				}

				expect_indentation(tokens, indentation);

				auto next = tokens.peek();
				if (next &&
					next->type == token_type::return_)
				{
					tokens.get();
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
