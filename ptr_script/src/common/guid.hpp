#ifndef GUID_HPP_INCLUDED_X8ZIAHCAOYUSICB1
#define GUID_HPP_INCLUDED_X8ZIAHCAOYUSICB1


#include <array>
#include <istream>
#include <ostream>
#include <exception>


namespace ptrs
{
	typedef unsigned char guid_byte;


	struct invalid_guid_error : std::runtime_error
	{
		invalid_guid_error();
	};


	struct guid
	{
		static const size_t size = 20;

		typedef std::array<guid_byte, size> Elements;


		Elements elements;


		///zero-fills the elements
		guid();
		explicit guid(const std::string &hexadecimalASCII);
		void fill(guid_byte value);
	};

	///returns true if all the elements are zero
	bool is_zero(const guid &guid);

	std::string to_string(const guid &guid);


	bool operator == (const guid &left, const guid &right);
	bool operator < (const guid &left, const guid &right);


	std::ostream &operator << (std::ostream &os, const guid &guid);
	std::istream &operator >> (std::istream &is, guid &guid);
}


#endif
