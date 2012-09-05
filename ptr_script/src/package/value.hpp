#ifndef VALUE_HPP_INCLUDED_YJZRTXMC635LUY4Q
#define VALUE_HPP_INCLUDED_YJZRTXMC635LUY4Q


#include "type.hpp"
#include <memory>


namespace ptrs
{
	struct type;


	struct value
	{
		explicit value(std::unique_ptr<ptrs::type> type);
		virtual ~value();
		const type &type() const;

	private:

		std::unique_ptr<ptrs::type> m_type;
	};
}


#endif
