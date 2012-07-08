#ifndef FCT_OBJECT_HPP
#define FCT_OBJECT_HPP


#include <ostream>
#include <memory>
#include <vector>


namespace fct
{
	struct Tree;
	struct Interpreter;


	struct Object
	{
		virtual ~Object();
		virtual void print(std::ostream &os) const = 0;
		virtual std::unique_ptr<Object> evaluate(
			Interpreter &interpreter,
			const std::vector<Tree> &arguments) const = 0;
		virtual bool equals(const Object &other) const = 0;
		virtual bool toBoolean() const;
	};


	bool operator == (const Object &left, const Object &right);
	bool operator != (const Object &left, const Object &right);

	std::ostream &operator << (std::ostream &os, const Object &object);
}


#endif