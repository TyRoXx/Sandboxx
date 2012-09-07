#ifndef SOURCE_HPP_INCLUDED_
#define SOURCE_HPP_INCLUDED_


#include <cstddef>
#include <string>
#include <vector>


namespace ptrs
{
	struct guid;


	namespace serialization
	{
		struct source
		{
			virtual ~source();
			virtual void integer(unsigned int &value) = 0;
			virtual void integer(unsigned long &value) = 0;
			virtual void integer(unsigned long long &value) = 0;
			virtual void integer(signed int &value) = 0;
			virtual void integer(signed long &value) = 0;
			virtual void integer(signed long long &value) = 0;
			virtual void identifier(std::string &identifier) = 0;
			virtual void data(std::vector<char> &data) = 0;
			virtual void text(std::string &text) = 0;
			virtual void guid(ptrs::guid &guid) = 0;
		};
	}
}


#endif
