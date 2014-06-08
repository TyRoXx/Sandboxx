#ifndef NEW_LANG_PARSER_HPP
#define NEW_LANG_PARSER_HPP

#include <silicium/source.hpp>
#include <silicium/sink.hpp>
#include <unordered_map>
#include <boost/variant.hpp>

namespace nl
{
	enum class token_type
	{
		dot,
		comma,
		left_parenthesis,
		right_parenthesis,
		assignment,
		space,
		tab,
		newline,
		identifier,
		integer,
		string,
		return_,
		end_of_file
	};

	char const *get_token_name(token_type token)
	{
		switch (token)
		{
#define NEW_LANG_RETURN_TOKEN_NAME(tk_) case token_type::tk_: return BOOST_STRINGIZE(tk_)
		NEW_LANG_RETURN_TOKEN_NAME(dot);
		NEW_LANG_RETURN_TOKEN_NAME(comma);
		NEW_LANG_RETURN_TOKEN_NAME(left_parenthesis);
		NEW_LANG_RETURN_TOKEN_NAME(right_parenthesis);
		NEW_LANG_RETURN_TOKEN_NAME(assignment);
		NEW_LANG_RETURN_TOKEN_NAME(space);
		NEW_LANG_RETURN_TOKEN_NAME(tab);
		NEW_LANG_RETURN_TOKEN_NAME(newline);
		NEW_LANG_RETURN_TOKEN_NAME(identifier);
		NEW_LANG_RETURN_TOKEN_NAME(integer);
		NEW_LANG_RETURN_TOKEN_NAME(string);
		NEW_LANG_RETURN_TOKEN_NAME(return_);
		NEW_LANG_RETURN_TOKEN_NAME(end_of_file);
#undef NEW_LANG_RETURN_TOKEN_NAME
		}
		return nullptr;
	}

	struct token
	{
		token_type type;
		std::string content;
	};

	inline bool is_decimal_digit(char c)
	{
		return (c >= '0') && (c <= '9');
	}

	inline bool is_identifier_head(char c)
	{
		return
			(c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			(c == '_') ||
			(c == '-');
	}

	inline bool is_identifier_middle(char c)
	{
		return is_identifier_head(c) || is_decimal_digit(c);
	}

	template <class Element>
	boost::optional<Element> peek(Si::source<Element> &source)
	{
		auto const next = source.map_next(1);
		if (next.empty())
		{
			return boost::none;
		}
		return next.front();
	}

	inline boost::optional<token> scan_token(Si::source<char> &input)
	{
		auto const first = Si::get(input);
		if (!first)
		{
			return token{token_type::end_of_file, ""};
		}
		static std::unordered_map<char, nl::token_type> const tokens =
		{
			{'.', nl::token_type::dot},
			{',', nl::token_type::comma},
			{'(', nl::token_type::left_parenthesis},
			{')', nl::token_type::right_parenthesis},
			{'=', nl::token_type::assignment},
			{' ', nl::token_type::space},
			{'\t', nl::token_type::tab},
			{'\n', nl::token_type::newline}
		};
		if (*first == '"')
		{
			std::string content;
			for (;;)
			{
				auto const next = Si::get(input);
				if (*next == '"')
				{
					return token{token_type::string, std::move(content)};
				}
				switch (*next)
				{
				case '\\':
					{
						auto const escaped = Si::get(input);
						if (!escaped)
						{
							return boost::none;
						}
						switch (*escaped)
						{
						case '\\':
						case '"':
							content.push_back(*escaped);
							break;

						default:
							return boost::none;
						}
						break;
					}

				default:
					content.push_back(*next);
					break;
				}
			}
		}
		{
			auto const single_char_found = tokens.find(*first);
			if (single_char_found != end(tokens))
			{
				return token{single_char_found->second, std::string(1, *first)};
			}
		}
		if (is_identifier_head(*first))
		{
			std::string content(1, *first);
			for (;;)
			{
				auto const next = peek(input);
				if (!next)
				{
					break;
				}
				if (!is_identifier_middle(*next))
				{
					break;
				}
				content.push_back(*next);
				Si::get(input);
			}
			if (content == "return")
			{
				return token{token_type::return_, std::move(content)};
			}
			return token{token_type::identifier, std::move(content)};
		}
		else if (is_decimal_digit(*first))
		{
			std::string content(1, *first);
			for (;;)
			{
				auto const next = peek(input);
				if (!next)
				{
					break;
				}
				if (!is_decimal_digit(*next))
				{
					break;
				}
				content.push_back(*next);
				Si::get(input);
			}
			return token{token_type::integer, std::move(content)};
		}
		return boost::none;
	}

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

		struct end_of_input
		{
		};

		struct unexpected_token
		{
			token unexpected;
		};

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
					expect_token(tokens, token_type::newline);
					auto body = parse_block(tokens, indentation + 1);
					return lambda{std::move(parameters), std::move(body)};
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
					expect_token(tokens, token_type::newline);
					return block{std::move(elements), std::move(result)};
				}

				elements.emplace_back(parse_definition(tokens, indentation));
			}
		}

		void print(Si::sink<char> &out, expression const &value, std::size_t indentation);

		inline void print_indentation(Si::sink<char> &out, std::size_t indentation)
		{
			for (size_t i = 0; i < indentation; ++i)
			{
				Si::append(out, "\t");
			}
		}

		struct expression_printer : boost::static_visitor<>
		{
			explicit expression_printer(Si::sink<char> &out, std::size_t indentation)
				: m_out(out)
				, m_indentation(indentation)
			{
			}

			void operator()(identifier const &value) const
			{
				Si::append(m_out, value.position.content);
			}

			void operator()(string const &value) const
			{
				Si::append(m_out, "\"");
				for (auto c : value.position.content)
				{
					switch (c)
					{
					case '\"':
						Si::append(m_out, "\\\"");
						break;

					case '\\':
						Si::append(m_out, "\\\\");
						break;

					default:
						Si::append(m_out, c);
					}
				}
				Si::append(m_out, "\"");
			}

			void operator()(integer const &value) const
			{
				Si::append(m_out, value.position.content);
			}

			void operator()(lambda const &value) const
			{
				Si::append(m_out, "(");
				for (auto p = begin(value.parameters); p != end(value.parameters); ++p)
				{
					auto const &parameter = *p;
					print(m_out, parameter.type, m_indentation);
					Si::append(m_out, " ");
					Si::append(m_out, parameter.name.content);
					if (p != (end(value.parameters) - 1))
					{
						Si::append(m_out, ", ");
					}
				}
				Si::append(m_out, ")\n");
				for (auto &definition : value.body.elements)
				{
					print_indentation(m_out, m_indentation + 1);
					Si::append(m_out, definition.name.content);
					Si::append(m_out, " = ");
					print(m_out, definition.value, m_indentation + 1);
					Si::append(m_out, "\n");
				}
			}

			void operator()(subscript const &value) const
			{
				print(m_out, value.left, m_indentation);
				Si::append(m_out, ".");
				Si::append(m_out, value.element.content);
			}

			void operator()(call const &value) const
			{
				print(m_out, value.function, m_indentation);
				Si::append(m_out, "(");
				for (auto a = begin(value.arguments); a != end(value.arguments); ++a)
				{
					print(m_out, *a, m_indentation);
					if (a != (end(value.arguments) - 1))
					{
						Si::append(m_out, ", ");
					}
				}
				Si::append(m_out, ")");
			}

		private:

			Si::sink<char> &m_out;
			std::size_t m_indentation;
		};

		inline void print(Si::sink<char> &out, expression const &value, std::size_t indentation)
		{
			return boost::apply_visitor(expression_printer(out, indentation), value);
		}
	}
}

#endif
