#ifndef FCT_SCANNER_HPP
#define FCT_SCANNER_HPP


#include "token.hpp"
#include <vector>


namespace fct
{
	typedef std::vector<Token> TokenSequence;
	typedef boost::iterator_range<SourceIterator> ScannerError;

	
	struct Scanner
	{
		static TokenSequence scan(
			const Source &source);
	};
}


#endif