#ifndef METHOD_TYPE_HPP_INCLUDED_3YOIBMODO4YL702N
#define METHOD_TYPE_HPP_INCLUDED_3YOIBMODO4YL702N


#include "type.hpp"
#include "structure_ref.hpp"
#include <vector>
#include <memory>


namespace ptrs
{
	struct method_type : type
	{
		typedef std::vector<std::unique_ptr<type>> type_vector;


		explicit method_type(
			const structure_ref &structure,
			type_vector parameters,
			type_vector results
			);
		virtual void accept(type_visitor &visitor) const PTR_SCRIPT_OVERRIDE;
		const structure_ref &structure() const;
		const type_vector &parameters() const;
		const type_vector &results() const;

	private:

		structure_ref m_structure;
		type_vector m_parameters;
		type_vector m_results;
	};
}


#endif
