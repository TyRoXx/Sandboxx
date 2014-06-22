#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "parser.hpp"
#include "analyze.hpp"
#include "interpreter.hpp"
#include <unordered_map>
#include <boost/lexical_cast.hpp>

BOOST_AUTO_TEST_CASE(scan_token_end_of_file)
{
	Si::memory_source<nl::source_char> empty;
	boost::optional<nl::token> scanned = nl::scan_token(empty);
	BOOST_REQUIRE(scanned);
	BOOST_CHECK(nl::token_type::end_of_file == scanned->type);
	BOOST_CHECK_EQUAL("", scanned->content);
}

namespace
{
	struct source_char_source : Si::source<nl::source_char>
	{
		explicit source_char_source(Si::source<char> &original)
			: original(original)
		{
		}

		virtual boost::iterator_range<nl::source_char const *> map_next(std::size_t) override
		{
			return {};
		}

		virtual nl::source_char *copy_next(boost::iterator_range<nl::source_char *> destination) override
		{
			auto d = boost::begin(destination);
			for (; d != boost::end(destination); ++d)
			{
				boost::optional<char> next = Si::get(original);
				if (!next)
				{
					break;
				}
				*d = nl::source_char{*next, current_pos};
				current_pos.column++;
				if (*next == '\n')
				{
					current_pos.column = 0;
					current_pos.line++;
				}
			}
			return d;
		}

		virtual boost::uintmax_t minimum_size() override
		{
			return original.minimum_size();
		}

		virtual boost::optional<boost::uintmax_t> maximum_size() override
		{
			return original.maximum_size();
		}

		virtual std::size_t skip(std::size_t count) override
		{
			std::size_t skipped = 0;
			for (; (Si::get(*this).is_initialized()) && (skipped < count); ++skipped)
			{
			}
			return skipped;
		}

	private:

		Si::source<char> &original;
		nl::character_position current_pos;
	};
}

BOOST_AUTO_TEST_CASE(source_char_source_trivial)
{
	std::string const input = ". a\n)=";
	Si::memory_source<char> raw_source(boost::make_iterator_range(input.data(), input.data() + input.size()));
	source_char_source non_buffered_source(raw_source);
	auto source = non_buffered_source | Si::buffered(1);
	std::vector<nl::source_char> const expected_chars
	{
		nl::source_char{'.', nl::character_position{0, 0}},
		nl::source_char{' ', nl::character_position{0, 1}},
		nl::source_char{'a', nl::character_position{0, 2}},
		nl::source_char{'\n', nl::character_position{0, 3}},
		nl::source_char{')', nl::character_position{1, 0}},
		nl::source_char{'=', nl::character_position{1, 1}}
	};
	for (auto const &expected : expected_chars)
	{
		boost::optional<nl::source_char> next = Si::get(source);
		BOOST_REQUIRE(next);
		BOOST_CHECK(expected == *next);
	}
	BOOST_CHECK(!Si::get(source));
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
	Si::memory_source<char> raw_source(boost::make_iterator_range(input.data(), input.data() + input.size()));
	source_char_source non_buffered_source(raw_source);
	auto source = non_buffered_source | Si::buffered(1);
	for (nl::token_type const expected : tokens)
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
	Si::memory_source<char> raw_source(boost::make_iterator_range(input.data(), input.data() + input.size()));
	source_char_source non_buffered_source(raw_source);
	auto source = non_buffered_source | Si::buffered(1);
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
	Si::memory_source<char> raw_source(boost::make_iterator_range(code.data(), code.data() + code.size()));
	source_char_source non_buffered_source(raw_source);
	auto source = non_buffered_source | Si::buffered(1);
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
	nl::ast::parser parser(buffer);
	handle(parser);
}

BOOST_AUTO_TEST_CASE(ast_parse_parameters)
{
	std::string const input = "uint32 a, unicode.code_point c)";
	with_tokenizer(input, [](nl::ast::parser &tokens)
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
	Si::memory_source<char> raw_source(boost::make_iterator_range(input.data(), input.data() + input.size()));
	source_char_source non_buffered_source(raw_source);
	auto source = non_buffered_source | Si::buffered(1);
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
	nl::ast::parser parser(buffer);
	auto parsed = nl::ast::parse_expression(parser, 0);
	nl::ast::lambda const * const lambda = boost::get<nl::ast::lambda>(&parsed);
	BOOST_REQUIRE(lambda);

	nl::character_position const irrelevant_position;

	nl::ast::lambda expected;
	expected.parameters.emplace_back(nl::ast::parameter{nl::ast::identifier{nl::token{nl::token_type::identifier, "uint32", irrelevant_position}}, nl::token{nl::token_type::identifier, "b", irrelevant_position}});
	expected.parameters.emplace_back(nl::ast::parameter{nl::ast::identifier{nl::token{nl::token_type::identifier, "string", irrelevant_position}}, nl::token{nl::token_type::identifier, "s", irrelevant_position}});
	expected.body.elements.emplace_back(nl::ast::definition{nl::token{nl::token_type::identifier, "a", irrelevant_position}, nl::ast::integer{nl::token{nl::token_type::integer, "2", irrelevant_position}}});
	expected.body.result = nl::ast::integer{nl::token{nl::token_type::integer, "1", irrelevant_position}};
	BOOST_CHECK_EQUAL(expected, *lambda);

	std::string back_to_str = boost::lexical_cast<std::string>(*lambda);
	BOOST_CHECK_EQUAL(input, back_to_str);
}

BOOST_AUTO_TEST_CASE(analyzer_lambda)
{
	nl::character_position const irrelevant_position;

	nl::il::external uint32{"uint32"};
	nl::il::name_space context;
	context.next = nullptr;
	context.definitions.insert(std::make_pair("uint32", nl::il::name_space_entry{nl::il::local_identifier{nl::il::local::definition, 0}, nl::il::null(), nl::il::type(uint32)}));
	nl::ast::lambda lambda;
	lambda.body.result = nl::ast::identifier{nl::token{nl::token_type::integer, "a", irrelevant_position}};
	lambda.parameters.emplace_back(nl::ast::parameter{nl::ast::identifier{nl::token{nl::token_type::identifier, "uint32", irrelevant_position}}, nl::token{nl::token_type::identifier, "a", irrelevant_position}});
	auto analyzed = nl::il::analyze(lambda, context, nullptr);
	nl::il::make_closure expected;
	expected.parameters.emplace_back(nl::il::parameter{uint32, "a"});
	expected.body.result = nl::il::local_expression{nl::il::local_identifier{nl::il::local::argument, 0}, uint32, "a", boost::none};
	BOOST_CHECK_EQUAL(nl::il::expression(expected), analyzed);
}

BOOST_AUTO_TEST_CASE(analyzer_argument_type_mismatch)
{
	nl::character_position const irrelevant_position;

	nl::il::external uint32{"uint32"};
	nl::il::external uint64{"uint64"};
	nl::il::external f{"f"};
	nl::il::name_space context;
	context.next = nullptr;
	context.definitions.insert(std::make_pair("uint32", nl::il::name_space_entry{nl::il::local_identifier{nl::il::local::definition, 0}, nl::il::null(), nl::il::type(uint32)}));
	auto const f_type = nl::il::type(nl::il::signature{uint32, {nl::il::type(uint64)}});
	context.definitions.insert(std::make_pair("f", nl::il::name_space_entry{nl::il::local_identifier{nl::il::local::definition, 1}, f_type, nl::il::value(f)}));
	nl::ast::lambda lambda;
	lambda.body.result = nl::ast::call{nl::ast::identifier{nl::token{nl::token_type::integer, "f", irrelevant_position}}, irrelevant_position, {nl::ast::identifier{nl::token{nl::token_type::integer, "a", irrelevant_position}}}};
	lambda.parameters.emplace_back(nl::ast::parameter{nl::ast::identifier{nl::token{nl::token_type::identifier, "uint32", irrelevant_position}}, nl::token{nl::token_type::identifier, "a", irrelevant_position}});
	BOOST_CHECK_EXCEPTION(nl::il::analyze(lambda, context, nullptr), std::runtime_error, [&](std::runtime_error const &ex)
	{
		auto const expected_message = nl::il::format_callability_error(nl::il::argument_type_not_applicable{0, uint64, uint32});
		BOOST_REQUIRE(expected_message);
		BOOST_REQUIRE_EQUAL(*expected_message + " (0:0)", ex.what());
		return true;
	});
}

namespace
{
	nl::ast::block parse(std::string const &code)
	{
		nl::ast::block result;
		with_tokenizer(code, [&result](nl::ast::parser &tokens)
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
	globals.definitions.insert(std::make_pair("f", nl::il::name_space_entry{nl::il::local_identifier{nl::il::local::bound, 0}, nl::il::signature{uint32, {uint32}}, f}));
	globals.definitions.insert(std::make_pair("g", nl::il::name_space_entry{nl::il::local_identifier{nl::il::local::bound, 1}, nl::il::signature{uint32, {}}, g}));

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

	template <class ResultHandler>
	void run_code(
			std::string const &code,
			nl::il::name_space global_info,
			std::vector<nl::interpreter::object_ptr> const &globals,
			ResultHandler const &handle_result)
	{
		auto const parsed = parse(code);
		nl::il::block const analyzed = nl::il::analyze_block(parsed, global_info);
		nl::interpreter::function const prepared = nl::interpreter::prepare_block(analyzed);
		nl::interpreter::closure const executable{prepared, globals};
		auto output = executable.call({});
		handle_result(std::move(output));
	}

	nl::interpreter::object_ptr run_code(
			std::string const &code,
			nl::il::name_space global_info,
			std::vector<nl::interpreter::object_ptr> const &globals)
	{
		nl::interpreter::object_ptr result;
		run_code(code, global_info, globals, [&result](nl::interpreter::object_ptr r)
		{
			result = std::move(r);
		});
		return result;
	}

	nl::il::value make_function(std::vector<nl::il::value> const &arguments)
	{
		if (arguments.empty())
		{
			throw std::invalid_argument("a function requires a result type");
		}
		return nl::il::signature{arguments.front(), std::vector<nl::il::value>(begin(arguments) + 1, end(arguments))};
	}

	nl::il::value my_type_of(std::vector<nl::il::value> const &arguments)
	{
		if (arguments.size() != 1)
		{
			throw std::runtime_error("typeof requires exactly one argument");
		}
		return nl::il::type_of_value(arguments[0]);
	}

	void add_constant(
			nl::il::name_space &analyzation_info,
			std::string const &name,
			nl::il::value const &constant)
	{
		std::size_t const id = analyzation_info.definitions.size();
		nl::il::name_space_entry entry
		{
			nl::il::local_identifier{nl::il::local::bound, id},
			nl::il::type_of_value(constant),
			constant
		};
		analyzation_info.definitions.insert(std::make_pair(name, std::move(entry)));
	}

	void add_external(
			nl::il::name_space &analyzation_info,
			std::vector<nl::interpreter::object_ptr> &execution_info,
			std::string const &name,
			nl::il::type const &type,
			nl::interpreter::object_ptr value
			)
	{
		std::size_t const id = execution_info.size();
		nl::il::name_space_entry entry
		{
			nl::il::local_identifier{nl::il::local::bound, id},
			type,
			boost::none
		};
		analyzation_info.definitions.insert(std::make_pair(name, std::move(entry)));
		execution_info.emplace_back(value);
	}

	void add_typeof(
			nl::il::name_space &analyzation_info,
			std::vector<nl::interpreter::object_ptr> &execution_info)
	{
		auto const my_type_of_type = nl::il::generic_signature{[](std::vector<nl::il::expression> const &) { return nl::il::meta_type{}; }, {[](nl::il::type const &) { return true; }}};
		add_constant(analyzation_info, "typeof", nl::il::compile_time_closure{my_type_of_type, my_type_of});
	}

	void test_hello_world_printing(std::string const &code)
	{
		auto const make_function_type = nl::il::signature{nl::il::signature_type{}, {nl::il::meta_type{}}};

		nl::il::value const print_operation{nl::il::external{"print_operation"}};

		nl::il::name_space global_info;
		global_info.next = nullptr;

		std::vector<nl::interpreter::object_ptr> globals;
		add_external(global_info, globals, "print_line", nl::il::signature{print_operation, {nl::il::string_type{}}}, make_functor(&print_line));
		add_constant(global_info, "string", nl::il::string_type{});
		add_constant(global_info, "function", nl::il::compile_time_closure{make_function_type, make_function});
		add_typeof(global_info, globals);

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

BOOST_AUTO_TEST_CASE(il_interpretation_6)
{
	std::string const code =
			"get_hello = ()\n"
			"	return \"Hello, world!\"\n"
			"string_generator = function(typeof(\"\"))\n"
			"call = (string_generator callee)\n"
			"	return callee()\n"
			"return print_line(call(get_hello))\n";
	test_hello_world_printing(code);
}

namespace
{
	nl::interpreter::object_ptr make_boolean(bool value)
	{
		return make_functor([value](std::vector<nl::interpreter::object_ptr> const &arguments) -> nl::interpreter::object_ptr
		{
			if (arguments.size() != 2)
			{
				throw std::invalid_argument("a bool has to be called with two arguments");
			}
			return arguments[!value];
		});
	}

	template <class UInt>
	struct uint_object : nl::interpreter::object
	{
		static_assert(std::is_unsigned<UInt>::value, "This class supports only unsigned integers");

		UInt const value;

		explicit uint_object(UInt value)
			: value(value)
		{
		}

		virtual nl::interpreter::object_ptr call(std::vector<nl::interpreter::object_ptr> const &) const SILICIUM_OVERRIDE
		{
			throw std::logic_error("uint cannot be called");
		}

		virtual nl::interpreter::object_ptr subscript(std::string const &element) const SILICIUM_OVERRIDE
		{
			if (element == "add" || element == "sub")
			{
				auto left = value;
				bool is_sub = (element == "sub");
				return make_functor([left, is_sub](std::vector<nl::interpreter::object_ptr> const &arguments) -> nl::interpreter::object_ptr
				{
					if (arguments.size() != 1)
					{
						throw std::invalid_argument("add requires exactly one argument");
					}
					auto const right_int = std::dynamic_pointer_cast<uint_object const>(arguments.front());
					if (!right_int)
					{
						throw std::invalid_argument("the argument to add has to be an integer");
					}
					auto right = right_int->value;
					if (is_sub)
					{
						right = static_cast<UInt>(-right);
					}
					auto result = static_cast<UInt>(left + right);
					return std::make_shared<uint_object>(result);
				});
			}
			else if (element == "less")
			{
				auto left = value;
				return make_functor([left](std::vector<nl::interpreter::object_ptr> const &arguments) -> nl::interpreter::object_ptr
				{
					if (arguments.size() != 1)
					{
						throw std::invalid_argument("less requires exactly one argument");
					}
					auto const right_int = std::dynamic_pointer_cast<uint_object const>(arguments.front());
					if (!right_int)
					{
						throw std::invalid_argument("the argument to less has to be an integer");
					}
					auto right = right_int->value;
					bool const is_less = (left < right);
					return make_boolean(is_less);
				});
			}
			throw std::invalid_argument("invalid element access on uint_object: " + element);
		}
	};

	template <class UInt>
	nl::interpreter::object_ptr make_uint(UInt value)
	{
		return std::make_shared<uint_object<UInt>>(value);
	}

	template <class UInt>
	nl::interpreter::object_ptr my_make_uint(std::vector<nl::interpreter::object_ptr> const &arguments)
	{
		if (arguments.size() != 1)
		{
			throw std::invalid_argument("add requires exactly one argument");
		}
		auto const value = std::dynamic_pointer_cast<nl::interpreter::value_object const>(arguments.front());
		if (!value)
		{
			throw std::invalid_argument("the argument to add has to be an integer literal");
		}
		auto const * const value_int = boost::get<nl::il::integer>(&value->value);
		if (!value_int)
		{
			throw std::invalid_argument("the argument to add has to be an integer literal");
		}
		auto const int_ = static_cast<UInt>(boost::lexical_cast<boost::uintmax_t>(value_int->value));
		return make_uint(int_);
	}
}

namespace
{
	void assign_uint_type(nl::il::type &destination)
	{
		nl::il::indirect_value self{&destination};
		nl::il::signature const branch{self, {}};
		nl::il::signature const boolean{branch, {branch, branch}};

		destination = nl::il::map
		{
			boost::unordered_map<nl::il::value, nl::il::value>
			{
				{nl::il::string{"add"}, nl::il::signature{self, {self}}},
				{nl::il::string{"sub"}, nl::il::signature{self, {self}}},
				{nl::il::string{"less"}, nl::il::signature{boolean, {self}}}
			}
		};
	}

	template <class UInt>
	void add_uint_type(
			nl::il::name_space &analyzation_info,
			std::vector<nl::interpreter::object_ptr> &execution_info,
			nl::il::indirect_value const &uint_type
			)
	{
		auto name = "uint" + boost::lexical_cast<std::string>(sizeof(UInt) * 8);
		add_external(analyzation_info, execution_info, "make_" + name, nl::il::signature{uint_type, {nl::il::integer_type{}}}, make_functor(my_make_uint<UInt>));
		add_constant(analyzation_info, name, uint_type);
	}
}

BOOST_AUTO_TEST_CASE(il_interpretation_subscript)
{
	std::string const code = "return i.add(make_uint8(1))\n";

	nl::il::value uint8_type;
	assign_uint_type(uint8_type);
	nl::il::value uint16_type;
	assign_uint_type(uint16_type);
	nl::il::value uint32_type;
	assign_uint_type(uint32_type);
	nl::il::value uint64_type;
	assign_uint_type(uint64_type);

	nl::il::name_space global_info;
	global_info.next = nullptr;

	std::vector<nl::interpreter::object_ptr> globals;
	add_external(global_info, globals, "i", uint8_type, make_uint<boost::uint8_t>(2));
	add_uint_type<boost::uint8_t>(global_info, globals, nl::il::indirect_value{&uint8_type});
	add_uint_type<boost::uint16_t>(global_info, globals, nl::il::indirect_value{&uint16_type});
	add_uint_type<boost::uint32_t>(global_info, globals, nl::il::indirect_value{&uint32_type});
	add_uint_type<boost::uint64_t>(global_info, globals, nl::il::indirect_value{&uint64_type});

	auto const output = run_code(code, global_info, globals);

	BOOST_REQUIRE(output);
	auto const result = std::dynamic_pointer_cast<uint_object<boost::uint8_t> const>(output);
	BOOST_REQUIRE(result);
	BOOST_CHECK(3 == result->value);
}

BOOST_AUTO_TEST_CASE(il_interpretation_self_recurse)
{
	std::string const code =
			"fib = (uint64 n) uint64\n"
			"	return_n = ()\n"
			"		return n\n"
			"	recurse = ()\n"
			"		first = fib(n.sub(make_uint64(2)))\n"
			"		second = fib(n.sub(make_uint64(1)))\n"
			"		return first.add(second)\n"
			"	return n.less(make_uint64(2))(return_n, recurse)()\n"
			"return fib(make_uint64(10))\n"
			;

	nl::il::value uint8_type;
	assign_uint_type(uint8_type);
	nl::il::value uint16_type;
	assign_uint_type(uint16_type);
	nl::il::value uint32_type;
	assign_uint_type(uint32_type);
	nl::il::value uint64_type;
	assign_uint_type(uint64_type);

	nl::il::name_space global_info;
	global_info.next = nullptr;

	std::vector<nl::interpreter::object_ptr> globals;
	add_external(global_info, globals, "i", uint8_type, make_uint<boost::uint8_t>(2));
	add_uint_type<boost::uint8_t>(global_info, globals, nl::il::indirect_value{&uint8_type});
	add_uint_type<boost::uint16_t>(global_info, globals, nl::il::indirect_value{&uint16_type});
	add_uint_type<boost::uint32_t>(global_info, globals, nl::il::indirect_value{&uint32_type});
	add_uint_type<boost::uint64_t>(global_info, globals, nl::il::indirect_value{&uint64_type});

	auto const output = run_code(code, global_info, globals);
	BOOST_REQUIRE(output);
	auto output_uint = std::dynamic_pointer_cast<uint_object<boost::uint64_t> const>(output);
	BOOST_REQUIRE(output_uint);
	BOOST_CHECK_EQUAL(55, output_uint->value);
}

BOOST_AUTO_TEST_CASE(il_analyze_non_const_explicit_return_type)
{
	std::string const code =
			"f = () make_uint64(0)\n"
			"	return f()\n"
			"return f\n";

	auto const parsed = parse(code);

	std::vector<nl::interpreter::object_ptr> dummy;

	nl::il::name_space global_info;
	global_info.next = nullptr;
	nl::il::value uint64_type;
	assign_uint_type(uint64_type);
	add_uint_type<boost::uint64_t>(global_info, dummy, nl::il::indirect_value{&uint64_type});

	BOOST_CHECK_EXCEPTION(nl::il::analyze_block(parsed, global_info), std::runtime_error, [](std::runtime_error const &e)
	{
		BOOST_REQUIRE_EQUAL("An explicit return type has to be a constant", std::string(e.what()));
		return true;
	});
}

BOOST_AUTO_TEST_CASE(il_analyze_missing_explicit_return_type)
{
	std::string const code =
			"f = ()\n"
			"	return f()\n"
			"return f\n";

	auto const parsed = parse(code);

	nl::il::name_space global_info;
	global_info.next = nullptr;
	nl::il::value uint64_type;
	assign_uint_type(uint64_type);
	add_constant(global_info, "uint64", uint64_type);

	BOOST_CHECK_EXCEPTION(nl::il::analyze_block(parsed, global_info), std::runtime_error, [](std::runtime_error const &e)
	{
		BOOST_REQUIRE_EQUAL("Unknown identifier f", std::string(e.what()));
		return true;
	});
}

BOOST_AUTO_TEST_CASE(il_analyze_wrong_explicit_return_type)
{
	std::string const code =
			"f = () uint64\n"
			"	return make_uint32(0)\n"
			"return f";

	auto const parsed = parse(code);

	std::vector<nl::interpreter::object_ptr> dummy;

	nl::il::name_space global_info;
	global_info.next = nullptr;

	nl::il::value uint32_type;
	assign_uint_type(uint32_type);
	add_uint_type<boost::uint32_t>(global_info, dummy, nl::il::indirect_value{&uint32_type});

	nl::il::value uint64_type;
	assign_uint_type(uint64_type);
	add_uint_type<boost::uint64_t>(global_info, dummy, nl::il::indirect_value{&uint64_type});

	BOOST_CHECK_EXCEPTION(nl::il::analyze_block(parsed, global_info), std::runtime_error, [](std::runtime_error const &e)
	{
		BOOST_REQUIRE_EQUAL("The return value type is not convertible into the explicit return type", std::string(e.what()));
		return true;
	});
}

namespace
{
	struct future : nl::interpreter::object
	{
		std::function<nl::interpreter::object_ptr()> action;

		explicit future(std::function<nl::interpreter::object_ptr()> action)
			: action(std::move(action))
		{
		}

		virtual nl::interpreter::object_ptr call(std::vector<nl::interpreter::object_ptr> const &) const override
		{
			throw std::logic_error("This object does not support the call operator");
		}

		virtual nl::interpreter::object_ptr subscript(std::string const &method_name) const
		{
			if (method_name == "then")
			{
				auto action_ = action;
				return make_functor([action_](std::vector<nl::interpreter::object_ptr> const &arguments) -> nl::interpreter::object_ptr
				{
					if (arguments.size() != 1)
					{
						throw std::invalid_argument("then requires one argument");
					}

					auto next_action = arguments[0];
					assert(next_action);

					auto next_ = [next_action, action_]() -> nl::interpreter::object_ptr
					{
						auto action_result = action_();
						assert(action_result);
						auto intermediate = next_action->call({action_result});
						assert(intermediate);
						auto intermediate_future = std::dynamic_pointer_cast<future const>(intermediate);
						assert(intermediate_future);
						return intermediate_future->action();
					};
					return std::make_shared<future>(next_);
				});
			}
			throw std::logic_error("Unknown method name");
		}
	};

	nl::interpreter::object_ptr my_async(std::vector<nl::interpreter::object_ptr> const &arguments)
	{
		if (arguments.size() != 1)
		{
			throw std::runtime_error("async requires exactly one argument");
		}
		auto action = arguments[0];
		return std::make_shared<future>(std::bind(&nl::interpreter::object::call, action, std::vector<nl::interpreter::object_ptr>()));
	}

	nl::il::value my_future(std::vector<nl::il::value> const &arguments)
	{
		assert(arguments.size() == 1);
		auto const element = arguments[0];
		nl::il::map future
		{
			boost::unordered_map<nl::il::value, nl::il::value>
			{
				{nl::il::string{"then"}, nl::il::generic_signature
					{
						[element](std::vector<nl::il::expression> const &arguments) -> nl::il::type
						{
							assert(arguments.size() == 1);
							auto const callback_type = nl::il::type_of_expression(arguments[0]);
							auto const callback_signature = nl::il::get_signature(callback_type);
							if (!callback_signature)
							{
								throw std::invalid_argument("The argument of future.then needs to be callable");
							}
							if (callback_signature->parameters.size() != 1)
							{
								throw std::invalid_argument("The argument of future.then needs to be a callback that takes one argument");
							}
							if (!nl::il::is_convertible(element, callback_signature->parameters[0]))
							{
								throw std::invalid_argument("The parameter of the callback provided to future.then is not compatible with the future type");
							}
							return my_future({callback_signature->result});
						},
						{[element](nl::il::type const &callback_type) -> bool
							{
								auto const callback_signature = nl::il::get_signature(callback_type);
								if (!callback_signature)
								{
									return false;
								}
								if (callback_signature->parameters.size() != 1)
								{
									return false;
								}
								auto const convertible = nl::il::determine_convertability(element, callback_signature->parameters[0]);
								auto const convert_error = nl::il::format_convertability_error(convertible);
								if (!convert_error)
								{
									return true;
								}
								return false;
							}
						}
					}
				}
			}
		};
		return future;
	}

	nl::il::type get_void_future()
	{
		nl::il::map void_;
		return my_future({void_});
	}

	nl::interpreter::object_ptr my_make_ready_future(std::vector<nl::interpreter::object_ptr> const &arguments)
	{
		assert(arguments.size() == 1);
		auto const &element = arguments[0];
		return make_functor([element](std::vector<nl::interpreter::object_ptr> const &) -> nl::interpreter::object_ptr
		{
			return std::make_shared<future>([element]() -> nl::interpreter::object_ptr
			{
				return element;
			});
		});
	}

	void add_async(
			nl::il::name_space &analyzation_info,
			std::vector<nl::interpreter::object_ptr> &execution_info)
	{
		nl::il::signature string_generator_type{nl::il::string_type{}, {}};
		nl::il::external string_future_type{"future(string)"};
		nl::il::signature async_type{string_future_type, {string_generator_type}};
		add_external(analyzation_info, execution_info, "async", async_type, make_functor(my_async));

		{
			nl::il::generic_signature const my_future_type{[](std::vector<nl::il::expression> const &) { return nl::il::meta_type{}; }, {[](nl::il::type const &) { return true; }}};
			add_constant(analyzation_info, "future", nl::il::compile_time_closure{my_future_type, my_future});
		}

		{
			add_external(
				analyzation_info,
				execution_info,
				"make_ready_future",
				nl::il::generic_signature
				{
					[](std::vector<nl::il::expression> const &arguments) -> nl::il::type
					{
						assert(arguments.size() == 1);
						auto const &element = nl::il::evaluate_const(arguments[0]);
						if (!element)
						{
							throw std::invalid_argument("The argument of make_ready_future needs to be a constant type");
						}
						return nl::il::signature{my_future({*element}), {*element}};
					},
					{[](nl::il::type const &) { return true; }}
				},
				make_functor(my_make_ready_future)
			);
		}
	}

	nl::interpreter::object_ptr my_print(std::vector<nl::interpreter::object_ptr> const &arguments, Si::sink<char> &print_stream)
	{
		if (arguments.size() != 1)
		{
			throw std::runtime_error("print requires exactly one argument");
		}
		auto value_obj = std::dynamic_pointer_cast<nl::interpreter::value_object const>(arguments[0]);
		assert(value_obj);
		auto printed = value_obj->value;
		return std::make_shared<future>([&print_stream, printed]() -> nl::interpreter::object_ptr
		{
			nl::il::print(print_stream, printed);
			return std::make_shared<future>([]() -> nl::interpreter::object_ptr
			{
				return std::make_shared<nl::interpreter::value_object>(nl::il::map{{}});
			});
		});
	}

	void add_print(
			nl::il::name_space &analyzation_info,
			std::vector<nl::interpreter::object_ptr> &execution_info,
			Si::sink<char> &print_stream)
	{
		nl::il::signature print_type{get_void_future(), {nl::il::string_type{}}};
		add_external(analyzation_info, execution_info, "print", print_type, make_functor(std::bind(my_print, std::placeholders::_1, std::ref(print_stream))));
	}
}

BOOST_AUTO_TEST_CASE(il_analyze_make_ready_future_0)
{
	std::vector<nl::interpreter::object_ptr> globals;
	nl::il::name_space global_info;
	global_info.next = nullptr;
	add_async(global_info, globals);
	add_typeof(global_info, globals);
	with_tokenizer("make_ready_future(typeof(\"\"))", [&](nl::ast::parser &parser)
	{
		auto expr_ast = nl::ast::parse_expression(parser, 0);
		auto expr = nl::il::analyze(expr_ast, global_info, nullptr);
		BOOST_CHECK(nl::il::type{(nl::il::signature{my_future({nl::il::string_type{}}), {nl::il::string_type{}}})} == nl::il::type_of_expression(expr));
	});
}

BOOST_AUTO_TEST_CASE(il_analyze_make_ready_future_1)
{
	std::vector<nl::interpreter::object_ptr> globals;
	nl::il::name_space global_info;
	global_info.next = nullptr;
	add_async(global_info, globals);
	add_typeof(global_info, globals);
	with_tokenizer("make_ready_future(typeof(\"\"))(\"str\")", [&](nl::ast::parser &parser)
	{
		auto expr_ast = nl::ast::parse_expression(parser, 0);
		auto expr = nl::il::analyze(expr_ast, global_info, nullptr);
		BOOST_CHECK(my_future({nl::il::string_type{}}) == nl::il::type_of_expression(expr));
	});
}

BOOST_AUTO_TEST_CASE(il_interpretation_hello_future)
{
	std::string const code =
			"return async(()\n"
			"	return \"Hello, future!\")\n";

	std::vector<nl::interpreter::object_ptr> globals;

	nl::il::name_space global_info;
	global_info.next = nullptr;

	add_async(global_info, globals);

	run_code(code, global_info, globals, [](nl::interpreter::object_ptr const &output)
	{
		BOOST_REQUIRE(output);

		auto const future_output = std::dynamic_pointer_cast<future const>(output);
		BOOST_REQUIRE(future_output);

		auto const message = future_output->action();
		BOOST_REQUIRE(message);

		auto const message_string = std::dynamic_pointer_cast<nl::interpreter::value_object const>(message);
		BOOST_REQUIRE(message_string);

		BOOST_CHECK(nl::il::value{nl::il::string{"Hello, future!"}} == message_string->value);
	});
}

BOOST_AUTO_TEST_CASE(il_interpretation_future_then)
{
	std::string const code =
			"return print(\"Hello\").then((void nothing)\n"
			"	return print(\", futur\")).then((void nothing)\n"
			"	return print(\"e\")).then((void nothing)\n"
			"	return print(\"!\"))\n"
			;

	std::vector<nl::interpreter::object_ptr> globals;

	nl::il::name_space global_info;
	global_info.next = nullptr;

	add_constant(global_info, "void", nl::il::map{{}});
	add_async(global_info, globals);

	std::string printed;
	auto print_stream = Si::make_container_sink(printed);
	add_print(global_info, globals, print_stream);

	run_code(code, global_info, globals, [](nl::interpreter::object_ptr const &output)
	{
		BOOST_REQUIRE(output);

		auto const future_output = std::dynamic_pointer_cast<future const>(output);
		BOOST_REQUIRE(future_output);

		auto const next = future_output->action();
		BOOST_REQUIRE(next);

		auto const next_future = std::dynamic_pointer_cast<future const>(next);
		BOOST_REQUIRE(next_future);

		auto const zero = next_future->action();
		BOOST_REQUIRE(zero);
	});

	BOOST_CHECK_EQUAL(printed, "Hello, future!");
}

namespace
{
	nl::il::value my_source(std::vector<nl::il::value> const &arguments)
	{
		if (arguments.size() != 1)
		{
			throw std::runtime_error("source requires one argument");
		}
		auto const &element = arguments[0];
		nl::il::signature const combination{element, {element, element}};
		nl::il::map source_methods
		{
			boost::unordered_map<nl::il::value, nl::il::value>
			{
				{nl::il::string{"accumulate"}, nl::il::signature{element, {element, combination}}}
			}
		};
		return source_methods;
	}

	void add_source(nl::il::name_space &analyzation_info,
					std::vector<nl::interpreter::object_ptr> &execution_info)
	{
		nl::il::generic_signature const source{[](std::vector<nl::il::expression> const &) { return nl::il::meta_type{}; }, {[](nl::il::type const &) { return true; }}};
		add_constant(analyzation_info, "source", nl::il::compile_time_closure{source, my_source});
	}

	struct source : nl::interpreter::object
	{
		typedef std::vector<nl::interpreter::object_ptr> object_source;

		explicit source(object_source const &next)
			: next(next)
		{
		}

		virtual nl::interpreter::object_ptr call(std::vector<nl::interpreter::object_ptr> const &) const override
		{
			throw std::logic_error("This object does not support the call operator");
		}

		virtual nl::interpreter::object_ptr subscript(std::string const &method_name) const override
		{
			assert(method_name == "accumulate");
			auto &next_ = next;
			return make_functor([&next_](std::vector<nl::interpreter::object_ptr> const &arguments)
			{
				assert(arguments.size() == 2);
				auto accumulator = arguments[0];
				auto combinator = arguments[1];
				for (auto &&element : next_)
				{
					accumulator = combinator->call({accumulator, element});
				}
				return accumulator;
			});
		}

	private:

		object_source const &next;
	};
}

BOOST_AUTO_TEST_CASE(il_interpretation_source_accumulate)
{
	std::string const code =
			"return (source(uint32) input)\n"
			"	combine = (uint32 first, uint32 second)\n"
			"		return first.add(second)\n"
			"	return input.accumulate(make_uint32(0), combine)\n"
			;

	std::vector<nl::interpreter::object_ptr> globals;

	nl::il::name_space global_info;
	global_info.next = nullptr;

	nl::il::value uint32_type;
	assign_uint_type(uint32_type);
	add_uint_type<boost::uint32_t>(global_info, globals, nl::il::indirect_value{&uint32_type});

	add_source(global_info, globals);

	run_code(code, global_info, globals, [](nl::interpreter::object_ptr const &output)
	{
		BOOST_REQUIRE(output);

		std::vector<nl::interpreter::object_ptr> const input
		{
			make_uint<boost::uint32_t>(1),
			make_uint<boost::uint32_t>(4),
			make_uint<boost::uint32_t>(8),
			make_uint<boost::uint32_t>(2)
		};
		auto const result = output->call({std::make_shared<source>(input)});
		BOOST_REQUIRE(result);
		auto const result_int = std::dynamic_pointer_cast<uint_object<boost::uint32_t> const>(result);
		BOOST_REQUIRE(result_int);
		BOOST_CHECK_EQUAL(15, result_int->value);
	});
}

namespace
{
	nl::il::type my_optional(std::vector<nl::il::type> const &arguments)
	{
		assert(arguments.size() == 1);
		auto const &element = arguments[0];
		nl::il::generic_signature::result_resolver const resolve = [element](std::vector<nl::il::expression> const &arguments) -> nl::il::type
		{
			assert(arguments.size());
			auto const &result = nl::il::evaluate_const(arguments[0]);
			if (!result)
			{
				throw std::invalid_argument("The argument of optional needs to be a constant type");
			}
			nl::il::signature const non_empty{*result, {element}};
			nl::il::signature const empty{*result, {}};
			return nl::il::signature{*result, {non_empty, empty}};
		};
		nl::il::map result
		{
			boost::unordered_map<nl::il::value, nl::il::value>
			{
				{nl::il::string{"branch"}, nl::il::generic_signature{resolve, {[](nl::il::type const &) { return true; }}}}
			}
		};
		return result;
	}

	void add_optional(
			nl::il::name_space &analyzation_info,
			std::vector<nl::interpreter::object_ptr> &execution_info)
	{
		add_constant(analyzation_info, "optional", nl::il::compile_time_closure{nl::il::generic_signature{[](std::vector<nl::il::expression> const &) { return nl::il::meta_type{}; }, {[](nl::il::type const &) { return true; }}}, my_optional});
	}

	struct some : nl::interpreter::object
	{
		explicit some(nl::interpreter::object_ptr value)
			: value(std::move(value))
		{
		}

		virtual nl::interpreter::object_ptr call(std::vector<nl::interpreter::object_ptr> const &) const override
		{
			throw std::logic_error("This object does not support the call operator");
		}

		virtual nl::interpreter::object_ptr subscript(std::string const &method_name) const override
		{
			assert(method_name == "branch");
			auto value_ = value;
			return make_functor([value_](std::vector<nl::interpreter::object_ptr> const &arguments)
			{
				assert(arguments.size() == 1);
				auto some_callback = arguments[0];
				return some_callback->call({value_});
			});
		}

	private:

		nl::interpreter::object_ptr value;
	};

	struct none : nl::interpreter::object
	{
		virtual nl::interpreter::object_ptr call(std::vector<nl::interpreter::object_ptr> const &) const override
		{
			throw std::logic_error("This object does not support the call operator");
		}

		virtual nl::interpreter::object_ptr subscript(std::string const &method_name) const override
		{
			assert(method_name == "branch");
			return make_functor([](std::vector<nl::interpreter::object_ptr> const &arguments)
			{
				assert(arguments.size() == 1);
				auto none_callback = arguments[1];
				return none_callback->call({});
			});
		}
	};

	nl::il::value my_istream(std::vector<nl::il::value> const &arguments)
	{
		assert(arguments.size());
		auto const &element = arguments[0];
		nl::il::map istream
		{
			boost::unordered_map<nl::il::value, nl::il::value>
			{
				{nl::il::string{"read"}, nl::il::signature{my_future({my_optional({element})}), {}}}
			}
		};
		return istream;
	}

	void add_istream(
			nl::il::name_space &analyzation_info,
			std::vector<nl::interpreter::object_ptr> &execution_info)
	{
		nl::il::generic_signature const my_istream_type{[](std::vector<nl::il::expression> const &) { return nl::il::meta_type{}; }, {[](nl::il::type const &) { return true; }}};
		add_constant(analyzation_info, "istream", nl::il::compile_time_closure{my_istream_type, my_istream});
	}

	struct istream : nl::interpreter::object
	{
		explicit istream(Si::source<nl::interpreter::object_ptr> &source)
			: source(source)
		{
		}

		virtual nl::interpreter::object_ptr call(std::vector<nl::interpreter::object_ptr> const &) const override
		{
			throw std::logic_error("This object does not support the call operator");
		}

		virtual nl::interpreter::object_ptr subscript(std::string const &method_name) const override
		{
			assert(method_name == "read");
			auto &source_ = source;
			return make_functor([&source_](std::vector<nl::interpreter::object_ptr> const &arguments) -> nl::interpreter::object_ptr
			{
				assert(arguments.empty());
				auto act = [&source_]() -> nl::interpreter::object_ptr
				{
					auto next = Si::get(source_);
					if (next)
					{
						return std::make_shared<some>(*next);
					}
					return std::make_shared<none>();
				};
				return std::make_shared<future>(act);
			});
		}

	private:

		Si::source<nl::interpreter::object_ptr> &source;
	};

	nl::il::value my_ostream(std::vector<nl::il::value> const &arguments)
	{
		assert(arguments.size());
		auto const &element = arguments[0];
		nl::il::map ostream
		{
			boost::unordered_map<nl::il::value, nl::il::value>
			{
				{nl::il::string{"write"}, nl::il::signature{get_void_future(), {element}}}
			}
		};
		return ostream;
	}

	void add_ostream(
			nl::il::name_space &analyzation_info,
			std::vector<nl::interpreter::object_ptr> &execution_info)
	{
		nl::il::generic_signature const my_ostream_type{[](std::vector<nl::il::expression> const &) { return nl::il::meta_type{}; }, {[](nl::il::type const &) { return true; }}};
		add_constant(analyzation_info, "ostream", nl::il::compile_time_closure{my_ostream_type, my_ostream});
	}

	struct ostream : nl::interpreter::object
	{
		explicit ostream(Si::sink<nl::interpreter::object_ptr> &sink)
			: sink(sink)
		{
		}

		virtual nl::interpreter::object_ptr call(std::vector<nl::interpreter::object_ptr> const &) const override
		{
			throw std::logic_error("This object does not support the call operator");
		}

		virtual nl::interpreter::object_ptr subscript(std::string const &method_name) const override
		{
			assert(method_name == "write");
			auto &sink_ = sink;
			return make_functor([&sink_](std::vector<nl::interpreter::object_ptr> const &arguments) -> nl::interpreter::object_ptr
			{
				assert(arguments.size() == 1);
				auto const written = arguments[0];
				auto act = [written, &sink_]() -> nl::interpreter::object_ptr
				{
					sink_.append(boost::make_iterator_range(&written, &written + 1));
					return std::make_shared<nl::interpreter::value_object>(nl::il::map{{}});
				};
				return std::make_shared<future>(act);
			});
		}

	private:

		Si::sink<nl::interpreter::object_ptr> &sink;
	};

	void add_boolean(
			nl::il::name_space &analyzation_info,
			std::vector<nl::interpreter::object_ptr> &execution_info)
	{
		add_constant(analyzation_info, "true", nl::il::string{"true"});
		add_constant(analyzation_info, "false", nl::il::string{"false"});
		add_constant(analyzation_info, "boolean", nl::il::string_type{});
	}
}

BOOST_AUTO_TEST_CASE(il_interpretation_stdio)
{
	std::string const code =
			"copy_element = (istream(uint32) in, ostream(uint32) out) future(boolean)\n"
			"	return in.read().then((optional(uint32) element) future(boolean)\n"
			"		got_sth = (uint32 element) future(boolean)\n"
			"			return out.write(element).then((void nothing) future(boolean)\n"
			"				return make_ready_future(boolean)(true))\n"
			"		got_nothing = () future(boolean)\n"
			"			return make_ready_future(boolean)(false)\n"
			"		return element.branch(future(boolean))(got_sth, got_nothing))\n"
			"return copy_element\n"
			;

	std::vector<nl::interpreter::object_ptr> globals;

	nl::il::name_space global_info;
	global_info.next = nullptr;

	nl::il::value uint32_type;
	assign_uint_type(uint32_type);
	add_uint_type<boost::uint32_t>(global_info, globals, nl::il::indirect_value{&uint32_type});

	add_constant(global_info, "void", nl::il::map{{}});
	add_source(global_info, globals);
	add_async(global_info, globals);
	add_istream(global_info, globals);
	add_ostream(global_info, globals);
	add_boolean(global_info, globals);
	add_optional(global_info, globals);

	run_code(code, global_info, globals, [](nl::interpreter::object_ptr const &output)
	{
		BOOST_REQUIRE(output);

		std::vector<boost::uint32_t> const originals{1, 2, 3};
		auto source = Si::make_container_source(originals);
		auto object_source = Si::make_transforming_source<nl::interpreter::object_ptr>(source, [](boost::uint32_t element)
		{
			return std::make_shared<uint_object<boost::uint32_t>>(element);
		});

		std::vector<nl::interpreter::object_ptr> copies;
		auto sink = Si::make_container_sink(copies);

		auto const result = output->call({std::make_shared<istream>(object_source), std::make_shared<ostream>(sink)});
		BOOST_REQUIRE(result);

		auto const result_future = std::dynamic_pointer_cast<future const>(result);
		BOOST_REQUIRE(result_future);
		BOOST_REQUIRE(copies.empty());

		auto const nothing = result_future->action();
		BOOST_REQUIRE(nothing);

		BOOST_REQUIRE(copies.size() == 1);
		BOOST_CHECK_EQUAL(2U, Si::get(source));
	});
}
