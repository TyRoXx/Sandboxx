#include "save_unit.hpp"
#include "unit.hpp"


namespace p0
{
	namespace intermediate
	{
		void save_unit(
			std::ostream &file,
			const unit &unit
			)
		{
			file.write(unit.name().data(), unit.name().size());
		}
	}
}
