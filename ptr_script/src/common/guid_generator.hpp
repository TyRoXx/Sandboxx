#ifndef GUID_GENERATOR_HPP_INCLUDED_WHXDGDBBZI7A4HRU
#define GUID_GENERATOR_HPP_INCLUDED_WHXDGDBBZI7A4HRU


#include "guid.hpp"
#include <random>


namespace ptrs
{
	struct guid_generator
	{
		template <class RNG>
		guid operator ()(RNG &rng) const
		{
			guid result;
			std::uniform_int<guid_byte> distribution;
			std::generate(
				result.elements.begin(),
				result.elements.end(),
				[&]()
			{
				return distribution(rng);
			});
			return result;
		}
	};
}


#endif
