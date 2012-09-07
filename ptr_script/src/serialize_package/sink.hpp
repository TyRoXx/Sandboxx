#ifndef SINK_HPP_INCLUDED_
#define SINK_HPP_INCLUDED_


#include <cstddef>
#include <string>


namespace ptrs
{
	struct guid;


	namespace serialization
	{
		struct sink
		{
			virtual ~sink();
			virtual void integer64(unsigned int value) = 0;
			virtual void integer64(unsigned long value) = 0;
			virtual void integer64(unsigned long long value) = 0;
			virtual void integer64(signed int value) = 0;
			virtual void integer64(signed long value) = 0;
			virtual void integer64(signed long long value) = 0;
			virtual void identifier(const std::string &identifier) = 0;
			virtual void data(const void *data, std::size_t size) = 0;
			virtual void text(const std::string &text) = 0;
			virtual void line() = 0;
			virtual void guid(const ptrs::guid &guid) = 0;
		};
	}
}


#endif
