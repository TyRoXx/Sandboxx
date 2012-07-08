#ifndef FCT_TOKEN_HPP
#define FCT_TOKEN_HPP


#include <string>
#include <boost/range/iterator_range.hpp>


namespace fct
{
	typedef std::string Source;
	typedef Source::const_iterator SourceIterator;
	typedef boost::iterator_range<SourceIterator> Token;
}


#endif