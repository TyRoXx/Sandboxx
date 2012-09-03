#ifndef METHOD_HPP_INCLUDED_QSASD85SBVO3O1Q0
#define METHOD_HPP_INCLUDED_QSASD85SBVO3O1Q0


#include <string>


namespace ptrs
{
	struct method
	{
		explicit method(
			std::string name
			);
		const std::string &name() const;
		
	private:
	
		std::string m_name;
	};
}


#endif
