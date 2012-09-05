#ifndef TYPE_VISITOR_HPP_INCLUDED_EDDY965Z6E7TBUJY
#define TYPE_VISITOR_HPP_INCLUDED_EDDY965Z6E7TBUJY


namespace ptrs
{
	struct ptr_type;
	struct structure_type;
	struct method_type;


	struct type_visitor
	{
		virtual ~type_visitor();
		virtual void accept(const ptr_type &type);
		virtual void accept(const structure_type &type);
		virtual void accept(const method_type &type);
	};
}


#endif
