#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "parser.hpp"
#include "analyze.hpp"
#include "interpreter.hpp"
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

template <class TokenizerHandler>
void with_tokenizer(std::string const &code, TokenizerHandler const &handle)
{
	Si::memory_source<char> source(boost::make_iterator_range(code.data(), code.data() + code.size()));
	auto lexer = Si::make_generator_source<nl::token>([&source]
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
	auto lexer = Si::make_generator_source<nl::token>([&source]
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

BOOST_AUTO_TEST_CASE(analyzer_lambda)
{
	nl::il::external uint32{"uint32"};
	nl::il::name_space context;
	context.next = nullptr;
	context.definitions.insert(std::make_pair("uint32", nl::il::name_space_entry{nl::il::local_identifier{nl::il::local::definition, 0}, nl::il::null(), nl::il::type(uint32)}));
	nl::ast::lambda lambda;
	lambda.body.result = nl::ast::identifier{nl::token{nl::token_type::integer, "a"}};
	lambda.parameters.emplace_back(nl::ast::parameter{nl::ast::identifier{nl::token{nl::token_type::identifier, "uint32"}}, nl::token{nl::token_type::identifier, "a"}});
	auto analyzed = nl::il::analyze(lambda, context);
	nl::il::make_closure expected;
	expected.parameters.emplace_back(nl::il::parameter{uint32, "a"});
	expected.body.result = nl::il::local_expression{nl::il::local_identifier{nl::il::local::argument, 0}, uint32, "a", boost::none};
	BOOST_CHECK_EQUAL(nl::il::expression(expected), analyzed);
}

BOOST_AUTO_TEST_CASE(analyzer_argument_type_mismatch)
{
	nl::il::external uint32{"uint32"};
	nl::il::external uint64{"uint64"};
	nl::il::external f{"f"};
	nl::il::name_space context;
	context.next = nullptr;
	context.definitions.insert(std::make_pair("uint32", nl::il::name_space_entry{nl::il::local_identifier{nl::il::local::definition, 0}, nl::il::null(), nl::il::type(uint32)}));
	context.definitions.insert(std::make_pair("f", nl::il::name_space_entry{nl::il::local_identifier{nl::il::local::definition, 1}, nl::il::type(nl::il::signature{uint32, {nl::il::type(uint64)}}), nl::il::value(f)}));
	nl::ast::lambda lambda;
	lambda.body.result = nl::ast::call{nl::ast::identifier{nl::token{nl::token_type::integer, "f"}}, {nl::ast::identifier{nl::token{nl::token_type::integer, "a"}}}};
	lambda.parameters.emplace_back(nl::ast::parameter{nl::ast::identifier{nl::token{nl::token_type::identifier, "uint32"}}, nl::token{nl::token_type::identifier, "a"}});
	BOOST_CHECK_EXCEPTION(nl::il::analyze(lambda, context), std::runtime_error, [](std::runtime_error const &ex)
	{
		return ex.what() == std::string("Argument type mismatch"); //TODO
	});
}

namespace
{
	nl::ast::block parse(std::string const &code)
	{
		nl::ast::block result;
		with_tokenizer(code, [&result](Si::source<nl::token> &tokens)
		{
			result = nl::ast::parse_block(tokens, 0);
		});
		return result;
	}
}

BOOST_AUTO_TEST_CASE(analyzer_chaining)
{
	std::string const code = "return f(g())\n";
	auto const parsed = parse(code);

	nl::il::type const uint32{nl::il::external{"uint32"}};
	nl::il::value const f{nl::il::external{"f"}};
	nl::il::value const g{nl::il::external{"g"}};

	nl::il::name_space globals;
	globals.next = nullptr;
	globals.definitions =
	{
		{"f", {nl::il::local_identifier{nl::il::local::bound, 0}, nl::il::signature{uint32, {uint32}}, f}},
		{"g", {nl::il::local_identifier{nl::il::local::bound, 1}, nl::il::signature{uint32, {}}, g}}
	};

	nl::il::block const analyzed = nl::il::analyze_block(parsed, globals);
	nl::il::block expected;
	auto g_call = nl::il::call{nl::il::local_expression{nl::il::local_identifier{nl::il::local::bound, 1}, nl::il::signature{uint32, {}}, "g", g}, {}};
	auto f_call = nl::il::call{nl::il::local_expression{nl::il::local_identifier{nl::il::local::bound, 0}, nl::il::signature{uint32, {uint32}}, "f", f}, {g_call}};
	expected.result = f_call;
	BOOST_CHECK_EQUAL(expected, analyzed);
}

namespace
{
	template <class F>
	struct functor : nl::interpreter::object
	{
		template <class G>
		explicit functor(G &&f)
			: f(std::forward<G>(f))
		{
		}

		virtual nl::interpreter::object_ptr call(std::vector<nl::interpreter::object_ptr> const &arguments) const SILICIUM_OVERRIDE
		{
			return f(arguments);
		}

	private:

		F f;
	};

	template <class F>
	nl::interpreter::object_ptr make_functor(F &&f)
	{
		typedef typename std::decay<F>::type clean_f;
		return std::make_shared<functor<clean_f>>(std::forward<F>(f));
	}

	struct print_operation_object : nl::interpreter::object
	{
		std::shared_ptr<nl::interpreter::value_object const> message;

		explicit print_operation_object(std::shared_ptr<nl::interpreter::value_object const> message)
			: message(std::move(message))
		{
		}

		virtual nl::interpreter::object_ptr call(std::vector<nl::interpreter::object_ptr> const &) const SILICIUM_OVERRIDE
		{
			throw std::logic_error("Cannot call this object");
		}
	};

	nl::interpreter::object_ptr print_line(std::vector<nl::interpreter::object_ptr> const &arguments)
	{
		if (arguments.size() != 1)
		{
			throw std::invalid_argument("Expected one argument");
		}
		auto const message = std::dynamic_pointer_cast<nl::interpreter::value_object const>(arguments[0]);
		if (!message)
		{
			throw std::invalid_argument("Expected a value_object argument");
		}
		return std::make_shared<print_operation_object>(message);
	}

	nl::interpreter::object_ptr run_code(
			std::string const &code,
			nl::il::name_space global_info,
			std::vector<nl::interpreter::object_ptr> const &globals)
	{
		auto const parsed = parse(code);
		nl::il::block const analyzed = nl::il::analyze_block(parsed, global_info);
		nl::interpreter::function const prepared = nl::interpreter::prepare_block(analyzed);
		nl::interpreter::closure const executable{prepared, globals};
		auto const output = executable.call({});
		return output;
	}

	nl::il::value make_function(std::vector<nl::il::value> const &arguments)
	{
		if (arguments.empty())
		{
			throw std::invalid_argument("a function requires a result type");
		}
		return nl::il::signature{arguments.front(), std::vector<nl::il::value>(begin(arguments) + 1, end(arguments))};
	}

	void test_hello_world_printing(std::string const &code)
	{
		auto const make_function_type = nl::il::signature{nl::il::signature_type{}, {nl::il::meta_type{}}};

		nl::il::name_space global_info;

		nl::il::value const print_line_symbol{nl::il::external{"print_line"}};
		nl::il::value const print_operation{nl::il::external{"print_operation"}};
		global_info.next = nullptr;
		global_info.definitions =
		{
			{
				"print_line",
				{nl::il::local_identifier{nl::il::local::bound, 0}, nl::il::signature{print_operation, {nl::il::string_type{}}}, print_line_symbol}
			},
			{
				"string",
				{nl::il::local_identifier{nl::il::local::bound, 1}, nl::il::meta_type{}, nl::il::value{nl::il::string_type{}}}
			},
			{
				"function",
				{nl::il::local_identifier{nl::il::local::bound, 2}, make_function_type, nl::il::value{nl::il::compile_time_closure{make_function_type, make_function}}}
			}
		};

		std::vector<nl::interpreter::object_ptr> const globals
		{
			make_functor(&print_line),
			nl::interpreter::object_ptr{}, //string
			nl::interpreter::object_ptr{} //function
		};
		auto const output = run_code(code, global_info, globals);

		BOOST_REQUIRE(output);
		auto const operation = std::dynamic_pointer_cast<print_operation_object const>(output);
		BOOST_REQUIRE(operation);
		BOOST_CHECK(nl::il::value(nl::il::string{"Hello, world!"}) == operation->message->value);
	}
}

BOOST_AUTO_TEST_CASE(il_interpretation_1)
{
	test_hello_world_printing("return print_line(\"Hello, world!\")\n");
}

BOOST_AUTO_TEST_CASE(il_interpretation_2)
{
	std::string const code =
			"print_hello = ()\n"
			"	return print_line(\"Hello, world!\")\n"
			"return print_hello()\n";
	test_hello_world_printing(code);
}

BOOST_AUTO_TEST_CASE(il_interpretation_3)
{
	std::string const code =
			"make_hello_printer = ()\n"
			"	return ()\n"
			"		return print_line(\"Hello, world!\")\n"
			"return make_hello_printer()()\n";
	test_hello_world_printing(code);
}

BOOST_AUTO_TEST_CASE(il_interpretation_4)
{
	std::string const code =
			"get_hello = ()\n"
			"	return \"Hello, world!\"\n"
			"call = (function(string) callee)\n"
			"	return callee()\n"
			"return print_line(call(get_hello))\n";
	test_hello_world_printing(code);
}

BOOST_AUTO_TEST_CASE(il_interpretation_5)
{
	std::string const code =
			"get_hello = ()\n"
			"	return \"Hello, world!\"\n"
			"string_generator = function(string)\n"
			"call = (string_generator callee)\n"
			"	return callee()\n"
			"return print_line(call(get_hello))\n";
	test_hello_world_printing(code);
}
