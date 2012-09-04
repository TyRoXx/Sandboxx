#ifndef ELEMENT_HPP_INCLUDED_2HPW9QABVY8HXPJC
#define ELEMENT_HPP_INCLUDED_2HPW9QABVY8HXPJC


#include "type.hpp"
#include <string>
#include <memory>


namespace ptrs
{
	struct element
	{
		explicit element(
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
