#include "conditional.hpp"
#include "statement_visitor.hpp"


namespace ptrs
{
	conditional::conditional(
		std::unique_ptr<value> condition,
		std::unique_ptr<statement> positive,
		std::unique_ptr<statement> negative
		)
		: m_condition(std::move(condition))
		, m_positive(std::move(positive))
		, m_negative(std::move(negative))
	{
	}

	void conditional::accept(statement_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	const value &conditional::condition() const
	{
		return *m_condition;
	}

	const statement &conditional::negative() const
	{
		return *m_negative;
	}

	const statement &conditional::positive() const
	{
		return *m_positive;
	}
}
