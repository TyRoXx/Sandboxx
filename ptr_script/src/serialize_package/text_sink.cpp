#include "text_sink.hpp"
#include "common/guid.hpp"


namespace ptrs
{
	namespace serialization
	{
		text_sink::text_sink(std::ostream &out)
			: m_out(out)
		{
		}

		void text_sink::integer(unsigned int value)
		{
			m_out << value << ' ';
		}

		void text_sink::integer(unsigned long value)
		{
			m_out << value << ' ';
		}

		void text_sink::integer(unsigned long long value)
		{
			m_out << value << ' ';
		}

		void text_sink::integer(signed int value)
		{
			m_out << value << ' ';
		}

		void text_sink::integer(signed long value)
		{
			m_out << value << ' ';
		}

		void text_sink::integer(signed long long value)
		{
			m_out << value << ' ';
		}

		void text_sink::identifier(const std::string &identifier)
		{
			m_out << identifier << ' ';
		}

		void text_sink::data(const void *data, std::size_t size)
		{

		}

		void text_sink::text(const std::string &text)
		{
			m_out << '\"';

			auto &out = m_out;
			const auto write_char = [&out](char c)
			{
				switch (c)
				{
				case '\n':
					out << "\\n";
					break;

				case '\r':
					out << "\\r";
					break;

				case '\t':
					out << "\\t";
					break;

				case '\"':
				case '\'':
				case '\\':
					out.put('\\');
					//fall through

				default:
					out.put(c);
					break;
				}
			};
			std::for_each(text.begin(), text.end(), write_char);

			m_out << '\"';
		}

		void text_sink::line()
		{
			m_out << '\n';
		}

		void text_sink::guid(const ptrs::guid &guid)
		{
			m_out << guid << ' ';
		}
	}
}
