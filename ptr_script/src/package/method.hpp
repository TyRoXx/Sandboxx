#ifndef METHOD_HPP_INCLUDED_QSASD85SBVO3O1Q0
#define METHOD_HPP_INCLUDED_QSASD85SBVO3O1Q0


#include "parameter.hpp"
#include <vector>


namespace ptrs
{
	struct method
	{
		typedef std::vector<parameter> parameter_vector;
		typedef std::vector<std::unique_ptr<type>> result_vector;


		explicit method(
			std::string name,
			parameter_vector parameters,
			result_vector results
			);
		const std::string &name() const;
		const parameter_vector &parameters() const;
		const result_vector &results() const;

	private:
	
		std::string m_name;
		parameter_vector m_parameters;
		result_vector m_results;
	};
}


#endif
