#ifndef SOURCE_HPP_INCLUDED_
#define SOURCE_HPP_INCLUDED_


#include <cstddef>
#include <string>
#include <vector>
#include <exception>


namespace ptrs
{
	struct guid;


	namespace serialization
	{
		struct parse_error : std::runtime_error
		{
			explicit parse_error(const std::string &message);
		};
		
		
		struct source
		{
			virtual ~source();
			virtual void integer64(unsigned int &value) = 0;
			virtual void integer64(unsigned long &value) = 0;
			virtual void integer64(unsigned long long &value) = 0;
			virtual void integer64(signed int &value) = 0;
			virtual void integer64(signed long &value) = 0;
			virtual void integer64(signed long long &value) = 0;
			virtual void identifier(std::string &identifier) = 0;
			virtual void data(std::vector<char> &data) = 0;
			virtual void text(std::string &text) = 0;
			virtual void guid(ptrs::guid &guid) = 0;
		};
	}
}


#endif
