#ifndef VALUE_HPP_INCLUDED_YJZRTXMC635LUY4Q
#define VALUE_HPP_INCLUDED_YJZRTXMC635LUY4Q


namespace ptrs
{
	struct type;


	struct value
	{
		explicit value(const ptrs::type &type);
		virtual ~value();
		type &type() const;

	private:

		const ptrs::type &m_type;
	};
}


#endif
