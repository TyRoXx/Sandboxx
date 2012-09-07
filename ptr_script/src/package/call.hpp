#ifndef CALL_HPP_INCLUDED_H49EBWL951P3VI5X
#define CALL_HPP_INCLUDED_H49EBWL951P3VI5X


#include "value.hpp"
#include <vector>
#include <memory>


namespace ptrs
{
	struct call : value
	{
		typedef std::vector<std::unique_ptr<value>> argument_vector;
		typedef std::vector<std::unique_ptr<value>> result_vector;


		explicit call(
			std::unique_ptr<value> method,
			argument_vector arguments,
			result_vector results
			);
		const value &method() const;
		const argument_vector &arguments() const;
		const result_vector &results() const;

	private:

		std::unique_ptr<value> m_method;
		argument_vector m_arguments;
		result_vector m_results;
	};
}


#endif
