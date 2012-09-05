#ifndef LITERAL_HPP_INCLUDED_YDBBUOGNI5QU1KM9
#define LITERAL_HPP_INCLUDED_YDBBUOGNI5QU1KM9


#include "value.hpp"
#include <boost/any.hpp>
#include <memory>


namespace ptrs
{
	struct literal : value
	{
		explicit literal(
			std::unique_ptr<ptrs::type> type,
			boost::any value
			);
		const boost::any &get() const;

	private:

		boost::any m_value;
	};
}


#endif
