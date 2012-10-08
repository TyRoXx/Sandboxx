#include "unit.hpp"


namespace p0
{
	namespace intermediate
	{
		unit::unit(
			std::string name
			)
			: m_name(std::move(name))
		{
		}

		const std::string &unit::name() const
		{
			return m_name;
		}
	}
}
