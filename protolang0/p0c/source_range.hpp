#pragma once
#ifndef P0C_SOURCE_RANGE_HPP
#define P0C_SOURCE_RANGE_HPP


#include <boost/range/iterator_range.hpp>


namespace p0
{
	typedef boost::iterator_range<const char *> source_range;
}


#endif
