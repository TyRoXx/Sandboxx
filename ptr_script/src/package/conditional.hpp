#ifndef CONDITIONAL_HPP_INCLUDED_MZUYRK7QCKFI34W0
#define CONDITIONAL_HPP_INCLUDED_MZUYRK7QCKFI34W0


#include "statement.hpp"
#include "value.hpp"
#include <memory>


namespace ptrs
{
	struct conditional : statement
	{
		explicit conditional(
			std::unique_ptr<value> condition,
			std::unique_ptr<statement> positive,
			std::unique_ptr<statement> negative
			);
		virtual void accept(statement_visitor &visitor) const PTR_SCRIPT_OVERRIDE;
		const value &condition() const;
		const statement &negative() const;
		const statement &positive() const;

	private:

		std::unique_ptr<value> m_condition;
		std::unique_ptr<statement> m_positive;
		std::unique_ptr<statement> m_negative;
	};
}


#endif
