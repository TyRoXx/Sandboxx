#include "guid.hpp"
#include <sstream>
#include <cctype>


namespace ptrs
{
	invalid_guid_error::invalid_guid_error()
		: std::runtime_error("The string cannot be parsed as a GUID")
	{
	}


	guid::guid()
	{
		elements.fill(0);
	}

	guid::guid(const std::string &hexadecimalASCII)
	{
		std::istringstream sstr(hexadecimalASCII);
		sstr >> *this;
		if (!sstr)
		{
			throw invalid_guid_error();
		}
	}

	void guid::fill(guid_byte value)
	{
		elements.fill(value);
	}


	bool is_zero(const guid &guid)
	{
		return std::all_of(
			guid.elements.begin(),
			guid.elements.end(),
			[](guid_byte e)
		{
			return (e == 0);
		});
	}

	std::string to_string(const guid &guid)
	{
		std::ostringstream sstr;
		sstr << guid;
		return sstr.str();
	}


	bool operator == (const guid &left, const guid &right)
	{
		return (left.elements == right.elements);
	}

	bool operator < (const guid &left, const guid &right)
	{
		return (left.elements < right.elements);
	}


	std::ostream &operator << (std::ostream &os, const guid &guid)
	{
		static const char * const Digits = "0123456789ABCDEF";

		for (auto i = guid.elements.begin(); i != guid.elements.end(); ++i)
		{
			const unsigned value = *i;
			os
				<< Digits[(value >> 4U) & 15U]
				<< Digits[value & 15U]
				;
		}

		return os;
	}

	namespace
	{
		unsigned hexidecimal_digit_value(char c)
		{
			switch (std::toupper(c))
			{
			case '0': return 0;
			case '1': return 1;
			case '2': return 2;
			case '3': return 3;
			case '4': return 4;
			case '5': return 5;
			case '6': return 6;
			case '7': return 7;
			case '8': return 8;
			case '9': return 9;
			case 'A': return 10;
			case 'B': return 11;
			case 'C': return 12;
			case 'D': return 13;
			case 'E': return 14;
			case 'F': return 15;
			default:
				return std::numeric_limits<unsigned>::max();
			}
		}
	}

	std::istream &operator >> (std::istream &is, guid &guid)
	{
		for (auto i = guid.elements.begin(); i != guid.elements.end(); ++i)
		{
			char first, second;
			is >> first >> second;
			if (!is)
			{
				break;
			}

			const unsigned high = hexidecimal_digit_value(first);
			const unsigned low = hexidecimal_digit_value(second);
			if (high >= 16 || low >= 16)
			{
				is.setf(std::ios::badbit);
				break;
			}

			*i = static_cast<guid_byte>((high << 4U) | low);
		}

		return is;
	}
}
