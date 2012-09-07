#ifndef TEXT_SINK_HPP_INCLUDED_
#define TEXT_SINK_HPP_INCLUDED_


#include "sink.hpp"
#include "common/override.hpp"
#include <ostream>


namespace ptrs
{
	namespace serialization
	{
		struct text_sink : sink
		{
			explicit text_sink(std::ostream &out);
			virtual void integer64(unsigned int value) PTR_SCRIPT_OVERRIDE;
			virtual void integer64(unsigned long value) PTR_SCRIPT_OVERRIDE;
			virtual void integer64(unsigned long long value) PTR_SCRIPT_OVERRIDE;
			virtual void integer64(signed int value) PTR_SCRIPT_OVERRIDE;
			virtual void integer64(signed long value) PTR_SCRIPT_OVERRIDE;
			virtual void integer64(signed long long value) PTR_SCRIPT_OVERRIDE;
			virtual void identifier(const std::string &identifier) PTR_SCRIPT_OVERRIDE;
			virtual void data(const void *data, std::size_t size) PTR_SCRIPT_OVERRIDE;
			virtual void text(const std::string &text) PTR_SCRIPT_OVERRIDE;
			virtual void line() PTR_SCRIPT_OVERRIDE;
			virtual void guid(const ptrs::guid &guid) PTR_SCRIPT_OVERRIDE;

		private:

			std::ostream &m_out;
		};
	}
}


#endif
