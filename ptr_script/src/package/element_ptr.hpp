#ifndef ELEMENT_PTR_HPP_INCLUDED_FMGXWII3ZC4GFLET
#define ELEMENT_PTR_HPP_INCLUDED_FMGXWII3ZC4GFLET


#include "pointer.hpp"
#include <memory>


namespace ptrs
{
	struct ptr_type;
	struct structure_type;


	struct element_ptr : pointer
	{
		explicit element_ptr(
			std::unique_ptr<ptr_type> type,
			std::unique_ptr<pointer> object,
			std::unique_ptr<structure_type> object_type,
			std::size_t element_index
			);
		const pointer &object() const;
		const structure_type &object_type() const;
		std::size_t element_index() const;
		
	private:
	
		std::unique_ptr<pointer> m_object;
		std::unique_ptr<structure_type> m_object_type;
		std::size_t m_element_index;
	};
}


#endif
