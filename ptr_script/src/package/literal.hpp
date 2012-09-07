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
			boost::any value
			);
		virtual void accept(value_visitor &visitor) const PTR_SCRIPT_OVERRIDE;
		const boost::any &get() const;

	private:

		boost::any m_value;
	};
}


#endif
