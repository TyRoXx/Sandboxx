#ifndef PARAMETER_HPP_INCLUDED_0U0OJDD0MOF7NK0E
#define PARAMETER_HPP_INCLUDED_0U0OJDD0MOF7NK0E


#include "type.hpp"
#include <string>
#include <memory>


namespace ptrs
{
	struct type;
	
	
	struct parameter
	{
		explicit parameter(
			std::unique_ptr<type> type,
			std::string name
			);
		const type &type() const;
		const std::string &name() const;
		
	private:
	
		std::unique_ptr<ptrs::type> m_type;
		std::string m_name;
	};
}


#endif
