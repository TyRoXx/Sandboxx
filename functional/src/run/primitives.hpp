#ifndef FCT_PRIMITIVES_HPP
#define FCT_PRIMITIVES_HPP


#include "object.hpp"
#include "program/tree.hpp"
#include <cstdint>


namespace fct
{
	struct Integer : Object
	{
		typedef std::uintmax_t Value;


		explicit Integer(Value value);
		Value getValue() const;
		virtual void print(std::ostream &os) const;
		virtual std::unique_ptr<Object> evaluate(
			Interpreter &interpreter,
			const std::vector<Tree> &arguments) const;
		virtual bool equals(const Object &other) const;
		virtual bool toBoolean() const;

	private:

		Value m_value;
	};


	struct Boolean : Object
	{
		explicit Boolean(bool value);
		virtual void print(std::ostream &os) const;
		virtual std::unique_ptr<Object> evaluate(
			Interpreter &interpreter,
			const std::vector<Tree> &arguments) const;
		virtual bool equals(const Object &other) const;
		virtual bool toBoolean() const;

	private:

		bool m_value;
	};


	struct Evaluate : Object
	{
		virtual void print(std::ostream &os) const;
		virtual std::unique_ptr<Object> evaluate(
			Interpreter &interpreter,
			const std::vector<Tree> &arguments) const;
		virtual bool equals(const Object &other) const;
	};


	struct Define : Object
	{
		virtual void print(std::ostream &os) const;
		virtual std::unique_ptr<Object> evaluate(
			Interpreter &interpreter,
			const std::vector<Tree> &arguments) const;
		virtual bool equals(const Object &other) const;
	};


	struct If : Object
	{
		virtual void print(std::ostream &os) const;
		virtual std::unique_ptr<Object> evaluate(
			Interpreter &interpreter,
			const std::vector<Tree> &arguments) const;
		virtual bool equals(const Object &other) const;
	};


	struct Not : Object
	{
		virtual void print(std::ostream &os) const;
		virtual std::unique_ptr<Object> evaluate(
			Interpreter &interpreter,
			const std::vector<Tree> &arguments) const;
		virtual bool equals(const Object &other) const;
	};


	struct LessThan : Object
	{
		virtual void print(std::ostream &os) const;
		virtual std::unique_ptr<Object> evaluate(
			Interpreter &interpreter,
			const std::vector<Tree> &arguments) const;
		virtual bool equals(const Object &other) const;
	};


	struct Add : Object
	{
		virtual void print(std::ostream &os) const;
		virtual std::unique_ptr<Object> evaluate(
			Interpreter &interpreter,
			const std::vector<Tree> &arguments) const;
		virtual bool equals(const Object &other) const;
	};


	struct Subtract : Object
	{
		virtual void print(std::ostream &os) const;
		virtual std::unique_ptr<Object> evaluate(
			Interpreter &interpreter,
			const std::vector<Tree> &arguments) const;
		virtual bool equals(const Object &other) const;
	};


	struct Function : Object
	{
		explicit Function(Tree value, std::vector<std::string> arguments);
		virtual void print(std::ostream &os) const;
		virtual std::unique_ptr<Object> evaluate(
			Interpreter &interpreter,
			const std::vector<Tree> &arguments) const;
		virtual bool equals(const Object &other) const;

	private:

		Tree m_value;
		std::vector<std::string> m_arguments;
	};


	struct MakeLambda : Object
	{
		virtual void print(std::ostream &os) const;
		virtual std::unique_ptr<Object> evaluate(
			Interpreter &interpreter,
			const std::vector<Tree> &arguments) const;
		virtual bool equals(const Object &other) const;
	};
}


#endif