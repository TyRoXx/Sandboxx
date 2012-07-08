#define BOOST_TEST_MODULE compile
#include <boost/test/unit_test.hpp>
#include <boost/format.hpp>

#include "compile/parser.hpp"
#include "run/interpreter.hpp"
#include "run/primitives.hpp"
using namespace fct;

namespace
{
	void checkTokens(
		const TokenSequence &found,
		const std::vector<std::string> &expected)
	{
		BOOST_REQUIRE_EQUAL(found.size(), expected.size());

		for (size_t i = 0; i < found.size(); ++i)
		{
			const auto token = found[i];
			BOOST_REQUIRE_EQUAL(
				std::string(begin(token), end(token)),
				expected[i]);
		}
	}
}

BOOST_AUTO_TEST_CASE(scanner)
{
	checkTokens(TokenSequence(), std::vector<std::string>());

	const std::string source = "a ab abc (a (ab )a ))a a()";
	checkTokens(
		TokenSequence(1, Token(begin(source), begin(source) + 1)),
		std::vector<std::string>(1, "a"));

	std::vector<std::string> expected;
	expected.push_back("a");
	expected.push_back("ab");
	expected.push_back("abc");
	expected.push_back("(");
	expected.push_back("a");
	expected.push_back("(");
	expected.push_back("ab");
	expected.push_back(")");
	expected.push_back("a");
	expected.push_back(")");
	expected.push_back(")");
	expected.push_back("a");
	expected.push_back("a");
	expected.push_back("(");
	expected.push_back(")");

	const auto tokens = Scanner::scan(
		source);
	checkTokens(tokens, expected);
}

namespace
{
	Token toToken(const std::string &str)
	{
		return Token(begin(str), end(str));
	}
}

BOOST_AUTO_TEST_CASE(parser)
{
	const std::string abc = "abc";
	const std::string lparen = "(";
	const std::string rparen = ")";
	TokenSequence tokens;
	tokens.push_back(toToken(abc));
	tokens.push_back(toToken(lparen));
	tokens.push_back(toToken(abc));
	tokens.push_back(toToken(rparen));

	const auto root = Parser::parse(tokens);
	const Tree expected("abc", std::vector<Tree>(1, Tree("abc")));
	BOOST_REQUIRE_EQUAL(root, expected);
}

BOOST_AUTO_TEST_CASE(interpreter_int)
{
	Tree program("1234");

	Interpreter interpreter;
	const auto result = interpreter.evaluate(program);
	BOOST_REQUIRE(result != 0);
	BOOST_REQUIRE_EQUAL(*result, Integer(1234));
}

BOOST_AUTO_TEST_CASE(interpreter_define_trivial)
{
	const std::string source = "define(x 3 x)";
	const Tree program = Parser::parse(Scanner::scan(source));

	Interpreter interpreter;
	interpreter.pushSymbol("define", std::unique_ptr<Object>(new Define));

	const auto result = interpreter.evaluate(program);
	BOOST_REQUIRE(result != 0);
	BOOST_REQUIRE_EQUAL(*result, Integer(3));
}

BOOST_AUTO_TEST_CASE(interpreter_define_layered)
{
	const std::string source = "define(x 1 define(y 2 define(x 3 x)))";
	const Tree program = Parser::parse(Scanner::scan(source));

	Interpreter interpreter;
	interpreter.pushSymbol("define", std::unique_ptr<Object>(new Define));

	const auto result = interpreter.evaluate(program);
	BOOST_REQUIRE(result != 0);
	BOOST_REQUIRE_EQUAL(*result, Integer(3));
}

BOOST_AUTO_TEST_CASE(interpreter_evaluate)
{
	const std::string source = "eval(eval(eval(3)))";
	const Tree program = Parser::parse(Scanner::scan(source));

	Interpreter interpreter;
	interpreter.pushSymbol("eval", std::unique_ptr<Object>(new Evaluate));

	const auto result = interpreter.evaluate(program);
	BOOST_REQUIRE(result != 0);
	BOOST_REQUIRE_EQUAL(*result, Integer(3));
}

BOOST_AUTO_TEST_CASE(interpreter_if)
{
	const std::string source = "if(1 if(0 4 3) 2)";
	const Tree program = Parser::parse(Scanner::scan(source));

	Interpreter interpreter;
	interpreter.pushSymbol("if", std::unique_ptr<Object>(new If));

	const auto result = interpreter.evaluate(program);
	BOOST_REQUIRE(result != 0);
	BOOST_REQUIRE_EQUAL(*result, Integer(3));
}

BOOST_AUTO_TEST_CASE(interpreter_not)
{
	const std::string source_0 = "not(0)";
	const std::string source_1 = "not(1)";
	const Tree program_0 = Parser::parse(Scanner::scan(source_0));
	const Tree program_1 = Parser::parse(Scanner::scan(source_1));

	Interpreter interpreter;
	interpreter.pushSymbol("not", std::unique_ptr<Object>(new Not));

	const auto result_0 = interpreter.evaluate(program_0);
	const auto result_1 = interpreter.evaluate(program_1);
	BOOST_REQUIRE(result_0 != 0);
	BOOST_REQUIRE(result_1 != 0);
	BOOST_REQUIRE_EQUAL(*result_0, Boolean(true));
	BOOST_REQUIRE_EQUAL(*result_1, Boolean(false));
}

BOOST_AUTO_TEST_CASE(interpreter_less_than)
{
	const std::string source = "less(2 3)";
	const Tree program = Parser::parse(Scanner::scan(source));

	Interpreter interpreter;
	interpreter.pushSymbol("less", std::unique_ptr<Object>(new LessThan));

	const auto result = interpreter.evaluate(program);
	BOOST_REQUIRE(result != 0);
	BOOST_REQUIRE_EQUAL(*result, Boolean(true));
}

BOOST_AUTO_TEST_CASE(interpreter_lambda)
{
	const auto greaterEqualTest = [](Integer::Value left, Integer::Value right, bool expected)
	{
		const std::string source = (boost::format(
			"define(greater-equal "
			"    lambda(left right"
			"        not(less(left right)))"
			"    greater-equal(%1% %2%)"
			")")
			% left
			% right).str();

		const Tree program = Parser::parse(Scanner::scan(source));

		Interpreter interpreter;
		interpreter.pushSymbol("define", std::unique_ptr<Object>(new Define));
		interpreter.pushSymbol("not", std::unique_ptr<Object>(new Not));
		interpreter.pushSymbol("lambda", std::unique_ptr<Object>(new MakeLambda));
		interpreter.pushSymbol("less", std::unique_ptr<Object>(new LessThan));

		const auto result = interpreter.evaluate(program);
		BOOST_REQUIRE(result != 0);
		BOOST_REQUIRE_EQUAL(*result, Boolean(expected));
	};

	greaterEqualTest(1, 1, true);
	greaterEqualTest(10, 3, true);
	greaterEqualTest(100, 5, true);
	greaterEqualTest(1, 1, true);
	greaterEqualTest(3, 10, false);
	greaterEqualTest(5, 100, false);
}
