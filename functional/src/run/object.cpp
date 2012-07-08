#include "object.hpp"


namespace fct
{
	Object::~Object()
	{
	}

	bool Object::toBoolean() const
	{
		return true;
	}


	bool operator == (const Object &left, const Object &right)
	{
		return left.equals(right);
	}

	bool operator != (const Object &left, const Object &right)
	{
		return !(left == right);
	}


	std::ostream &operator << (std::ostream &os, const Object &object)
	{
		object.print(os);
		return os;
	}
}
