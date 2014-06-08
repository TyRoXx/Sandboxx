#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "parser.hpp"
#include <unordered_map>
#include <boost/lexical_cast.hpp>

BOOST_AUTO_TEST_CASE(scan_token_end_of_file)
{
	Si::memory_source<char> empty;
	boost::optional<nl::token> scanned = nl::scan_token(empty);
	BOOST_REQUIRE(scanned);
	BOOST_CHECK(nl::token_type::end_of_file == scanned->type);
	BOOST_CHECK_EQUAL("", scanned->content);
}

BOOST_AUTO_TEST_CASE(scan_token_sequence)
{
	std::string const input = ".,()= \t\n\"string\"identifier 123 return";
	auto const tokens =
	{
		nl::token_type::dot,
		nl::token_type::comma,
		nl::token_type::left_parenthesis,
		nl::token_type::right_parenthesis,
		nl::token_type::assignment,
		nl::token_type::space,
		nl::token_type::tab,
		nl::token_type::newline,
		nl::token_type::string,
		nl::token_type::identifier,
		nl::token_type::space,
		nl::token_type::integer,
		nl::token_type::space,
		nl::token_type::return_,
		nl::token_type::end_of_file
	};
	Si::memory_source<char> source(boost::make_iterator_range(input.data(), input.data() + input.size()));
	for (auto const expected : tokens)
	{
		boost::optional<nl::token> const scanned = nl::scan_token(source);
		BOOST_REQUIRE(scanned);
		BOOST_CHECK_EQUAL(static_cast<int>(expected), static_cast<int>(scanned->type));
	}
	BOOST_CHECK(!Si::get(source));
}

BOOST_AUTO_TEST_CASE(scan_token_string)
{
	std::string const input = "\"abc123\\\"\\\\\"";
	Si::memory_source<char> source(boost::make_iterator_range(input.data(), input.data() + input.size()));
	boost::optional<nl::token> scanned = nl::scan_token(source);
	BOOST_REQUIRE(scanned);
	BOOST_CHECK(nl::token_type::string == scanned->type);
	BOOST_CHECK_EQUAL("abc123\"\\", scanned->content);

	boost::optional<nl::token> eof = nl::scan_token(source);
	BOOST_REQUIRE(eof);
	BOOST_CHECK(nl::token_type::end_of_file == eof->type);
	BOOST_CHECK_EQUAL("", eof->content);
}

template <class Element, class Generator>
struct generator_source : Si::source<Element>
{
	explicit generator_source(Generator generate_next)
		: m_generate_next(std::move(generate_next))
	{
	}

	virtual boost::iterator_range<Element const *> map_next(std::size_t size) SILICIUM_OVERRIDE
	{
		(void)size;
		return boost::iterator_range<Element const *>();
	}

	virtual Element *copy_next(boost::iterator_range<Element *> destination) SILICIUM_OVERRIDE
	{
		auto copied = destination.begin();
		for (; copied != destination.end(); ++copied)
		{
			auto generated = m_generate_next();
			if (!generated)
			{
				break;
			}
			*copied = std::move(*generated);
		}
		return copied;
	}

	virtual boost::uintmax_t minimum_size() SILICIUM_OVERRIDE
	{
		return 0;
	}

	virtual boost::optional<boost::uintmax_t> maximum_size() SILICIUM_OVERRIDE
	{
		return boost::none;
	}

	virtual std::size_t skip(std::size_t count) SILICIUM_OVERRIDE
	{
		std::size_t i = 0;
		for (; i < count; ++i)
		{
			if (!m_generate_next())
			{
				break;
			}
		}
		return i;
	}

private:

	Generator m_generate_next;
};

template <class Element, class Generator>
generator_source<Element, typename std::decay<Generator>::type> make_generator_source(Generator &&generate_next)
{
	return generator_source<Element, typename std::decay<Generator>::type>{std::forward<Generator>(generate_next)};
}

template <class TokenizerHandler>
void with_tokenizer(std::string const &code, TokenizerHandler const &handle)
{
	Si::memory_source<char> source(boost::make_iterator_range(code.data(), code.data() + code.size()));
	auto lexer = make_generator_source<nl::token>([&source]
	{
		auto token = nl::scan_token(source);
		if (!token)
		{
			throw std::runtime_error("lexer failure");
		}
		return token;
	});
	Si::buffering_source<nl::token> buffer(lexer, 1);
	handle(buffer);
}

BOOST_AUTO_TEST_CASE(ast_parse_parameters)
{
	std::string const input = "uint32 a, unicode.code_point c)";
	with_tokenizer(input, [](Si::source<nl::token> &tokens)
	{
		auto parsed = nl::ast::parse_parameters(tokens, 0);
		BOOST_REQUIRE_EQUAL(2, parsed.size());
	});
}

BOOST_AUTO_TEST_CASE(ast_lambda)
{
	std::string const input =
			"(uint32 b, string s)\n"
			"\ta = 2\n"
			"\treturn 1\n"
			;
	Si::memory_source<char> source(boost::make_iterator_range(input.data(), input.data() + input.size()));
	auto lexer = make_generator_source<nl::token>([&source]
	{
		auto token = nl::scan_token(source);
		if (!token)
		{
			throw std::runtime_error("lexer failure");
		}
		return token;
	});
	Si::buffering_source<nl::token> buffer(lexer, 1);
	auto parsed = nl::ast::parse_expression(buffer, 0);
	nl::ast::lambda const * const lambda = boost::get<nl::ast::lambda>(&parsed);
	BOOST_REQUIRE(lambda);

	nl::ast::lambda expected;
	expected.parameters.emplace_back(nl::ast::parameter{nl::ast::identifier{nl::token{nl::token_type::identifier, "uint32"}}, nl::token{nl::token_type::identifier, "b"}});
	expected.parameters.emplace_back(nl::ast::parameter{nl::ast::identifier{nl::token{nl::token_type::identifier, "string"}}, nl::token{nl::token_type::identifier, "s"}});
	expected.body.elements.emplace_back(nl::ast::definition{nl::token{nl::token_type::identifier, "a"}, nl::ast::integer{nl::token{nl::token_type::integer, "2"}}});
	expected.body.result = nl::ast::integer{nl::token{nl::token_type::integer, "1"}};
	BOOST_CHECK_EQUAL(expected, *lambda);

	std::string back_to_str = boost::lexical_cast<std::string>(*lambda);
	BOOST_CHECK_EQUAL(input, back_to_str);
}
