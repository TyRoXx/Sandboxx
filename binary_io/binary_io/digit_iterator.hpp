#ifndef BINARY_IO_DIGIT_ITERATOR_HPP_
#define BINARY_IO_DIGIT_ITERATOR_HPP_


#include <cassert>
#include <climits>
#include <boost/iterator/iterator_facade.hpp>


namespace bio
{
	template <class Endianness, class T, class Digit = unsigned char, std::size_t Size = sizeof(T) / sizeof(Digit)>
	struct digit_iterator : boost::iterator_facade<digit_iterator<Endianness, T, Digit, Size>, Digit, boost::random_access_traversal_tag, Digit>
	{
		explicit digit_iterator(
			T value,
			std::size_t start = 0)
			: m_value(std::move(value))
			, m_position(start)
		{
		}

		digit_iterator()
			: m_position(Size)
		{
		}
		
		Digit dereference() const
		{
			return static_cast<Digit>(m_value >> get_shift_bits());
		}

		bool equal(const digit_iterator &other) const
		{
			return (m_position == other.m_position);
		}

		void increment()
		{
			++m_position;
			assert(m_position <= Size);
		}

		void decrement()
		{
			assert(m_position > 0);
			--m_position;
		}

		std::ptrdiff_t distance_to(const digit_iterator &other) const
		{
			return (other.m_position - m_position);
		}

	private:

		T m_value;
		std::size_t m_position;


		std::size_t get_shift_bits() const
		{
			const std::size_t index = Endianness::get_shift(m_position, Size);
			return (index * sizeof(Digit) * CHAR_BIT);
		}
	};
}


#endif
