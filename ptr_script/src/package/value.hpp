#ifndef VALUE_HPP_INCLUDED_YJZRTXMC635LUY4Q
#define VALUE_HPP_INCLUDED_YJZRTXMC635LUY4Q


#include <ostream>


namespace ptrs
{
	struct type;


	struct value
	{
		explicit value(const ptrs::type &type);
		virtual ~value();
		type &type() const;
		virtual void print(std::ostream &os) const = 0;

	private:

		const ptrs::type &m_type;
	};
}


#endif
