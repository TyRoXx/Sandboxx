#ifndef ELEMENT_PTR_HPP_INCLUDED_FMGXWII3ZC4GFLET
#define ELEMENT_PTR_HPP_INCLUDED_FMGXWII3ZC4GFLET


#include "value.hpp"
#include <memory>


namespace ptrs
{
	struct ptr_type;


	struct element_ptr : value
	{
		explicit element_ptr(
			std::unique_ptr<value> object,
			std::size_t element_index
			);
		virtual void accept(value_visitor &visitor) const PTR_SCRIPT_OVERRIDE;
		const value &object() const;
		std::size_t element_index() const;
		
	private:
	
		std::unique_ptr<value> m_object;
		std::size_t m_element_index;
	};
}


#endif
